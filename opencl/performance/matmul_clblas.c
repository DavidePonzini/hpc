#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// USAGE: matmul <smaller> <larger> <step>
// OUTPUT: PERFORMANCE IN GFLOPS
// compile with gcc -O2 -lclBLAS -lOpenCL matmul_opencl.c

// https://github.com/clMathLibraries/clBLAS

#include <clBLAS.h>


#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000000 /* 5 seconds */


int main ( int argc, char **argv ) {

	int i,j,k,n,p,nmin,nmax,step;
	struct timespec tstart,t1,t2;
	double ttstart,tt1,tt2,diff,min;

	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_uint n_platforms = 0, n_devices = 0;
	cl_context_properties properties[3];
	cl_int err = CL_SUCCESS;
	clblasStatus status;
	cl_context context;
	cl_command_queue queue;
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
	
	err = clblasSetup();
	if (err != CL_SUCCESS){
		fprintf(stderr,"error in clBLAS setup\n");
		return 1;
	};
	
/* performance evaluation for all matrix size from nmin to nmax */
	for (n=nmin; n<=nmax; n += step) {

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

	status = clblasDgemm( clblasRowMajor, clblasNoTrans, clblasNoTrans,
		n, n, n, 1, a_bff, 0, n, b_bff, 0, n, 0, c_bff, 0, n,
		1, &queue, 0, NULL, NULL );

	if (status != CL_SUCCESS){
		fprintf(stderr,"error running cblas routine\n");
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
/* ...at least one trial... */
			if (p == 0) continue;
/* ...and at most TRIALS times, but no longer than MAXTIME */
			if ((tt2 - ttstart) > MAXTIME)
				break;
		}

/* print performance in GFLOPS */
		double dn = (double)n;
		fprintf(stderr,"%u %g\n",n,(2*dn*dn*dn-dn*dn)/min);

	}

	clblasTeardown();
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;

}
