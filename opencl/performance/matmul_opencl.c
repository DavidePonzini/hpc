#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <CL/cl.h>


// USAGE: matmul <smaller> <larger> <step>
// OUTPUT: PERFORMANCE IN GFLOPS
// compile with gcc -O2 -lOpenCL matmul_opencl.c

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000000 /* 5 seconds */


const char *source1 =
"kernel void matmul_item(global double *a, global double *b, global double *c, int p, int n) {"
"  /* the number m of rows in a[] is unneeded */			"
"									"
"  /* get matrix indices from work-item 2-D coordinates.         */	"
"  /* note we must NOT reverse the order:                        */	"
"  /* 2-D pattern has been specified matrix-style, not cartesian */	"
"  int i = get_global_id(0);						"
"  int j = get_global_id(1);						"
"  int k;								"
"  /* use a local var for the temporary sum, it's faster */		"
"  double s = 0;								"
"  for (k=0; k<p; k++)							"
"    s += a[i*p+k] * b[k*n+j];						"
"  /* store the result to destination (in global mem) */		"
"  c[i*n+j] = s;							"
"}									";


#define BLOCK 15

const char *source2 =
"#define BLOCK 15\n    /* always append a slash-n to #define's */	"
"									"
"kernel void matmul_item(global double *a, global double *b, global double *c, int p, int n) {"
"  /* the number m of rows in a[] is unneeded */			"
"									"
"  int k, kk, num_blocks;						"
"  /* from global coordinates to matrix indices */			"
"  int i = get_global_id(0);						"
"  int j = get_global_id(1);						"
"  /* from group coordinates to block indices */			"
"  int ii = get_local_id(0);						"
"  int jj = get_local_id(1);						"
"  /* recall: coords are matrix-style, not cartesian */			"
"									"
"  /* local mem for matrix blocks, it's faster */			"
"  local double a_blk[BLOCK*BLOCK];					"
"  local double b_blk[BLOCK*BLOCK];					"
"  local double c_blk[BLOCK*BLOCK];					"
"									"
"  /* init c_blk to zero, in parallel by all group members */		"
"  c_blk[ii*BLOCK + jj] = 0;						"
"									"
"  /* outer loop onto blocks */						"
"  num_blocks = p/BLOCK;						"
"  for (k=0; k < num_blocks; k++) {					"
"      /* copy k-th block from global mem into local mem.  */		"
"      /* done in parallel by all group members */			"
"      a_blk[ii*BLOCK + jj] = a[i*p + (k*BLOCK + jj)];			"
"      b_blk[ii*BLOCK + jj] = b[(k*BLOCK + ii) * n + j];		"
"      /* ensure copy is done before using the data */			"
"      barrier(CLK_LOCAL_MEM_FENCE);					"
"      /* inner loop within block.                 */			"
"      /* [ii,jj] in parallel by all group members */			"
"      for (kk=0; kk<BLOCK; kk++)					"
"          c_blk[ii*BLOCK + jj] +=					"
"              a_blk[ii*BLOCK + kk] * b_blk[kk*BLOCK + jj];		"
"      /* ensure computation done before overwriting blocks */		"
"      barrier(CLK_LOCAL_MEM_FENCE);					"
"  }									"
"									"
"  /* c_blk is complete --> store back to global mem. */		"
"  /* in parallel as usual by all group members       */		"
"  c[i*n + j] = c_blk[ii*BLOCK + jj];					"
"}									";


int main ( int argc, char **argv ) {

	int i,j,k,n,p,nmin,nmax,step;
	struct timespec tstart,t1,t2;
	double ttstart,tt1,tt2,diff,min;

	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_uint n_platforms = 0, n_devices = 0;
	cl_context_properties properties[3];
	cl_int err = CL_SUCCESS;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	cl_mem a_bff, b_bff, c_bff;	
	cl_double *a,*b,*c;


	if (argc < 4) return 0;
	nmin = atoi(argv[1]);
	nmax = atoi(argv[2]);
	step = atoi(argv[3]);
	if (nmin < 1 || nmin > nmax || step < 1) return 0;

/* allocate the three matrices and align to cache lines */
	a = (cl_double *)malloc(nmax*nmax*sizeof(cl_double)+CACHELINE);
	b = (cl_double *)malloc(nmax*nmax*sizeof(cl_double)+CACHELINE);
	c = (cl_double *)malloc(nmax*nmax*sizeof(cl_double)+CACHELINE);
	a = (cl_double *)(((unsigned long)a+CACHELINE)&~(CACHELINE-1));
	b = (cl_double *)(((unsigned long)b+CACHELINE)&~(CACHELINE-1));
	c = (cl_double *)(((unsigned long)c+CACHELINE)&~(CACHELINE-1));

/* initialize A and B */
	for (i=0;i<nmax;i++)
		for (j=0;j<nmax;j++) {
			a[i*nmax+j] = j;
			b[i*nmax+j] = i;
		}

	if (clGetPlatformIDs(1,&platform_id,&n_platforms) != CL_SUCCESS) {
		fprintf(stderr,"error: no platform\n");
		return 1;
	};

//	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, &n_devices) != CL_SUCCESS)
	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &n_devices) != CL_SUCCESS)
	{
		fprintf(stderr,"error: no device\n");
		return 1;
	};

        properties[0]= CL_CONTEXT_PLATFORM;
        properties[1]= (cl_context_properties) platform_id;
        properties[2]= 0;
	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error creating context\n");
		return 1;
	};

	queue = clCreateCommandQueueWithProperties(context, device_id, (cl_command_queue_properties)0, &err);
	
//	program = clCreateProgramWithSource(context, 1, (const char**)&source1, NULL, &err);
	program = clCreateProgramWithSource(context, 1, (const char**)&source2, NULL, &err);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error creating kernel program\n");
		return 1;
	};
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error building kernel program\n");
		return 1;
	};
	kernel = clCreateKernel(program, "matmul_item", &err);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error creating kernel obj\n");
		return 1;
	};
	
/* performance evaluation for all matrix size from nmin to nmax */
	for (n=nmin; n<=nmax; n += step) {

		for (;n % BLOCK; n++);
		if (n > nmax) break;

		size_t global_pattern[] = {n,n,0};
		size_t group_pattern[] = {BLOCK,BLOCK,0};

		min = 1.0e100;

		clock_gettime(CLOCK_REALTIME,&tstart);

		ttstart = (double)tstart.tv_sec*1.0e9 + (double)tstart.tv_nsec;

/* for each matrix size, run at most TRIALS times */
		for (p=0; p<TRIALS; p++) {

			clock_gettime(CLOCK_REALTIME,&t1);

	a_bff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, n * n * sizeof(cl_double), a, NULL);
	b_bff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, n * n * sizeof(cl_double), b, NULL);
// this should be fine but doesn't work...and I don't know why...
//	c_bff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, n * n * sizeof(cl_double), c, NULL);
	c_bff = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, n * n * sizeof(cl_double), c, NULL);
	err = clSetKernelArg(kernel, 0, sizeof(a_bff), &a_bff);
	err |= clSetKernelArg(kernel, 1, sizeof(b_bff), &b_bff);
	err |= clSetKernelArg(kernel, 2, sizeof(c_bff), &c_bff);
	err |= clSetKernelArg(kernel, 3, sizeof(n), &n);
	err |= clSetKernelArg(kernel, 4, sizeof(n), &n);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error passing parameters to kernel\n");
		return 1;
	};

        // copy input matrices to device.  Surprise: this can be skipped,
        // because the device makes a much better "copy on demand" when data
        // are actually needed.
        // Enable it if you want to measure performance of kernel
        // excluding data movement overhead from the measurement.
#if 0
	err = clEnqueueWriteBuffer(queue, a_bff, CL_FALSE, 0, n * n * sizeof(cl_double), a, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(queue, b_bff, CL_FALSE, 0, n * n * sizeof(cl_double), b, 0, NULL, NULL);
	// be sure the transfer is done before running the kernel
	err = clFinish(queue);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error passing input data\n");
		return 1;
	};
#endif

	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_pattern, group_pattern, 0, NULL, NULL);
// Questa qui sotto dovrebbe andar bene nel caso memoria globale perche'
// i gruppi non vengono usati, ma invece ha un impatto estremamente negativo
// sulle prestazioni.  Dunque e' opportuno definire il pattern dei gruppi
// anche se poi non li si usa.
//	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_pattern, NULL, 0, NULL, NULL);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error running kernel\n");
		return 1;
	};
	
	// retrieve results.
        // we select a **blocking** behaviour (CL_TRUE) to be sure we get the
        // result.  Cmds are executed in-order, so when this cmd has finished,
        // the previous one has finished as well.
        // That's why the clFinish() below can be skipped.
        // Enable the clFinish() if you want to measure kernel performance
        // excluding data movement overhead

//	err = clFinish(queue);

	err = clEnqueueReadBuffer(queue, c_bff, CL_TRUE, 0, n * n * sizeof(cl_double), c, 0, NULL, NULL);
	if (err != CL_SUCCESS){
		fprintf(stderr,"error getting results\n");
		return 1;
	};

	clReleaseMemObject(a_bff);
	clReleaseMemObject(b_bff);
	clReleaseMemObject(c_bff);

			clock_gettime(CLOCK_REALTIME,&t2);

			tt1 = (double)t1.tv_sec * 1.0e9 + (double)t1.tv_nsec;
			tt2 = (double)t2.tv_sec * 1.0e9 + (double)t2.tv_nsec;
			diff = tt2 - tt1;
/* take the best performance result */
			if (diff < min) min = diff;
/* ...at most TRIALS times no longer than MAXTIME */
			if ((tt2 - ttstart) > MAXTIME)
				break;
		}

/* print performance in GFLOPS */
		double dn = (double)n;
		fprintf(stderr,"%u %g\n",n,(2*dn*dn*dn-dn*dn)/min);

	}

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;

}
