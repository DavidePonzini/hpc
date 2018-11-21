//
// a commented example of OpenCL C program.
// thanks to David Black-Schaffer for the original version.
//
// compile with "g++ -std=c++11 -lOpenCL"
//
// online reference manual for the various OpenCL functions:
// http://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/
// for understanding the meaning of the NULL parameters here and there ;-)
//

#include <iostream>
#include <ctime>
#include <chrono>

// header for OpenCL C programs
#include <CL/cl.h>

using namespace std;

#define LENGTH 1000000

// define our kernel. it just calculates the sin of the input data.
// this kernel is a separate program embedded as a string in the main program.
const char *source =
"kernel void calcSin(global double *data) {				"
"  /* get this work item id. relative to global (1-D) array */		"
"  int id = get_global_id(0);						"
"  /* do the computation of this work item */				"
"  data[id] = sin(data[id]);						"
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
    cl_mem buffer;	

    chrono::high_resolution_clock::time_point t_start,t_end;
    chrono::duration<double> exec_time;

    // create and initialize the input data
    cl_double *data;
    data = new cl_double[LENGTH];
    for (int i=0; i<LENGTH; i++) 
        data[i] = i;

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
    // queue_properties bitmask determine for instance whether the queue is
    // out-of-order.  default is in-order, and it is ok for us, so we put
    // a zero bitmask.
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

    // creating the kernel executable, with "calcSin" as the entry point

    kernel = clCreateKernel(program, "calcSin", &err);
    if (err != CL_SUCCESS){
        cerr << "error creating kernel executable\n";
        return 1;
    };

    // specify a decomposition pattern (job --> work-items)
    // we choose a 1-D space with LENGTH work-items
    // (one-to-one mapping to data elems)

    size_t global_pattern[] = {LENGTH,0,0};

    // optionally (but not required in our example) we can specify
    // that our work-items must be organized in groups, according to
    // some pattern.

    size_t *group_pattern = NULL;

    ///////////////////////////////////////////////
    t_start = chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////

    // pass the parameters to the kernel routine

    buffer = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, LENGTH * sizeof(cl_double), data, &err);
    if (err != CL_SUCCESS){
        cerr << "error creating mem buff for kernel args\n";
        return 1;
    };

    err = clSetKernelArg(kernel, 0, sizeof(buffer), &buffer);
    if (err != CL_SUCCESS){
        cerr << "error passing args to kernel routine\n";
        return 1;
    };
	
    // issue a "run the kernel" cmd to the device queue. "1" is for 1-D space
    // it is a non-blocking cmd; the last parameter (NULL here) could provide
    // a handle for subsequent waiting.
    // see https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clEnqueueNDRangeKernel.html

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_pattern, group_pattern, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        cerr << "error running kernel\n";
        return 1;
    };

    // issue a "read buffer" cmd, so we get the results.
    // we select a **blocking** behaviour (CL_TRUE) to be sure we get the
    // result (cmds are executed in-order; when this cmd has finished,
    // the previous one must have finished as well)

    err = clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, LENGTH * sizeof(cl_double), data, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        cerr << "error getting results\n";
        return 1;
    };

    /////////////////////////////////////////////
    t_end = chrono::high_resolution_clock::now();
    /////////////////////////////////////////////

    // clean up
    clReleaseMemObject(buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // print results
    // for (int i=0; i<LENGTH; i++)
    //	printf("sin(%d) = %f\n", i, data[i]);

    exec_time = t_end - t_start;

    cout << "execution time: " << exec_time.count() * 1e6 << " usec\n";

    delete [] data;

}
