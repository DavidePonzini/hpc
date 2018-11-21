//
// naive matrix multiplication performed in blocks by groups of work-items
//
// C = A * B
// where A[m x p], B[p x n], C[m x n]
//
// compile with "g++ -std=c++11 -lOpenCL"
//
// output: performance in GFLOPS
//
// online reference manual for the various OpenCL functions:
// http://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/
// for understanding the meaning of the NULL parameters here and there ;-)
//


#include <iostream>
#include <ctime>
#include <chrono>

#include <CL/cl.h>

using namespace std;


// the "magic number" 15 has been found experimentally on my AMD Radeon
// R9 390 and is probably valid only with this specific GPU.

#define BLOCK 15

const char *source =
"#define BLOCK 15\n    /* always append a slash-n to #define's */	"
"									"
"kernel void matmul_item(global double *a, global double *b, global double *c, int p, int n) {"
"  /* the number m of rows in a[] is unneeded */			"
"									"
"  int k, kk;								"
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
"									"
"  double cc = 0;							"
"									"
"  /* outer loop onto blocks */						"
"  for (k = 0; k < p; k += BLOCK) {					"
"      /* copy k-th block from global mem into local mem.  */		"
"      /* iteration space [ii,jj] in parallel by all group members */	"
"      a_blk[ii*BLOCK + jj] = a[i*p + k + jj];				"
"      b_blk[ii*BLOCK + jj] = b[(k + ii)*n + j];			"
"      /* ensure copy is done before using the data */			"
"      barrier(CLK_LOCAL_MEM_FENCE);					"
"      /* row x column within block */					"
"      for (kk = 0; kk < BLOCK; kk++)					"
"          cc += a_blk[ii*BLOCK + kk] * b_blk[kk*BLOCK + jj];		"
"      /* ensure computation done before overwriting blocks */		"
"      barrier(CLK_LOCAL_MEM_FENCE);					"
"  }									"
"									"
"  c[i*n + j] = cc;							"
"									"
"}									";


int main (int argc, const char * argv[]) {

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

    chrono::high_resolution_clock::time_point t_start,t_end;
    chrono::duration<double> exec_time;

    // A[m x p], B[p x n], C[m x n]
    cl_double *a, *b, *c;
    int m, p, n;
    int i, j, k;


    if (argc < 4) {
        fprintf(stderr,"error: program needs three args\n");
        return 1;
    }

    m = atoi(argv[1]);
    p = atoi(argv[2]);
    n = atoi(argv[3]);

    if (m % BLOCK || p % BLOCK || n % BLOCK) {
        cerr << "error: matrix dims are not multiple of " << BLOCK << endl;
        return 1;
    }

    a = new cl_double[m * p];
    b = new cl_double[p * n];
    c = new cl_double[m * n];

    for (i=0; i<m; i++)
        for (j=0; j<p; j++)
            a[i*p+j] = 1;
    for (i=0; i<p; i++)
        for (j=0; j<n; j++)
            b[i*n+j] = 1;

    // get 1 platform
    if (clGetPlatformIDs(1,&platform_id,&n_platforms) != CL_SUCCESS) {
        cerr << "error: no platform\n";
        return 1;
    };

    // on that platform, get 1 device of desired kind
    // (GPU, or CPU; we could also use both)

//    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, &n_devices) != CL_SUCCESS)
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &n_devices) != CL_SUCCESS)
    {
        cerr << "error: no device\n";
        return 1;
    };

    // create a context for device.  properties list must end with 0

    properties[0]= CL_CONTEXT_PLATFORM;
    properties[1]= (cl_context_properties) platform_id;
    properties[2]= 0;

    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS){
        cerr << "error creating context\n";
        return 1;
    };

    // a queue for sending commands from host to device.

    queue = clCreateCommandQueueWithProperties(context, device_id, (cl_command_queue_properties)0, &err);

    // build the kernel program:
    // load kernel source, build kernel executable, create kernel object

    program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    if (err != CL_SUCCESS){
        cerr << "error creating kernel program\n";
        return 1;
    };

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS){
        cerr << "error building kernel program\n";
        return 1;
    };

    kernel = clCreateKernel(program, "matmul_item", &err);
    if (err != CL_SUCCESS){
        cerr << "error creating kernel executable\n";
        return 1;
    };

    // specify a decomposition pattern (job --> work-items):
    // 2-D space with [m x n] work-items (one-to-one mapping to C[m x n]).
    // NOTE it is matrix style, not cartesian.
    // no group pattern at this time

    size_t global_pattern[] = {(size_t)m,(size_t)n,0};

    // specify a group pattern (work-items --> work-groups)

    size_t group_pattern[] = {BLOCK,BLOCK,0};

    ///////////////////////////////////////////////
    t_start = chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////

    // create & pass parameters to kernel

    a_bff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, m * p * sizeof(cl_double), a, &err);

    b_bff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, p * n * sizeof(cl_double), b, &err);

// this should be fine but doesn't work...and I don't know why...
//    c_bff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, m * n * sizeof(cl_double), c, &err);
    c_bff = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, m * n * sizeof(cl_double), c, &err);

    err |= clSetKernelArg(kernel, 0, sizeof(a_bff), &a_bff);
    err |= clSetKernelArg(kernel, 1, sizeof(b_bff), &b_bff);
    err |= clSetKernelArg(kernel, 2, sizeof(c_bff), &c_bff);
    err |= clSetKernelArg(kernel, 3, sizeof(p), &p);
    err |= clSetKernelArg(kernel, 4, sizeof(n), &n);
    if (err != CL_SUCCESS){
        cerr << "error creating or passing parameters to kernel\n";
        return 1;
    };

    // copy input matrices to device.  THIS HOWEVER CAN BE SKIPPED,
    // because the device makes a much better "copy on demand" if and when
    // it actually needs the data.
    // Enable it if you want to measure performance of kernel
    // excluding data movement overhead from the measurement.
#if 0
    err = clEnqueueWriteBuffer(queue, a_bff, CL_FALSE, 0, m * n * sizeof(cl_double), a, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, b_bff, CL_FALSE, 0, m * n * sizeof(cl_double), b, 0, NULL, NULL);
    // be sure the transfer is done before running the kernel
    err = clFinish(queue);
    if (err != CL_SUCCESS){
        cerr << "error passing input data\n";
        return 1;
    };
#endif

    // run the kernel.  "2" means "2-D decomposition".

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_pattern, group_pattern, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        cerr << "error running kernel\n";
        return 1;
    };

    // retrieve results.
    // we select a **blocking** behaviour (CL_TRUE) to be sure we get the
    // result (cmds are executed in-order; when this cmd has finished,
    // the previous one has finished as well).
    // That's why the clFinish() below can be skipped.
    // Enable the clFinish() if you want to measure kernel performance
    // excluding data movement overhead

//    err = clFinish(queue);

    err = clEnqueueReadBuffer(queue, c_bff, CL_TRUE, 0, m * n * sizeof(cl_double), c, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        cerr << "error getting results\n";
        return 1;
    };

    clReleaseMemObject(a_bff);
    clReleaseMemObject(b_bff);
    clReleaseMemObject(c_bff);

    /////////////////////////////////////////////
    t_end = chrono::high_resolution_clock::now();
    /////////////////////////////////////////////

    // clean up
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // print results
//    for (i=0; i<m; i++) {
//        for (j=0; j<n; j++)
//            cout << c[i*n+j] << " ";
//        cout << endl;
//    }

    exec_time = t_end - t_start;

    // print performance in GFLOPS

    double dm = (double)m;
    double dp = (double)p;
    double dn = (double)n;
    cout << n << " " << (2*dp*dm*dn-dm*dn)/(exec_time.count()*1e9) << endl;

    delete [] a;
    delete [] b;
    delete [] c;

    return 0;

}
