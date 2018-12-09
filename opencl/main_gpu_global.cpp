#include <iostream>
#include <string>

#include <ctime>
#include <chrono>

#include "file.h"

#include <CL/cl.h>


using namespace std;

#define SKIP_OVERHEAD

#define BLOCK 10

#ifndef TRIALS
	#define TRIALS 1
#endif
double min_exec_time = 1.0e100;
double min_mem_w_time = 1.0e100;
double min_mem_r_time = 1.0e100;


const char* source =
"kernel void gpu_compute(global double* m_in, global double* m_out, int size_i, int size_j, double p, double discr) {"
"	int i = get_global_id(0);"
"	int ip = i*size_j;"
"	int j = get_global_id(1);"

	// skip edges
"	if(i != 0 && j != 0 && i != size_i-1 && j != size_j-1) {"
		// compute result
"		m_out[ip + j] = m_in[ip + j]*(1.0-4.0*discr*p) + discr*p*("
"			m_in[ip-size_j + j] +"
"			m_in[ip+size_j + j] +"
"			m_in[ip + j-1] +"
"			m_in[ip + j+1]);"
"		}"
"}";


//#define DEBUG_KERNEL_BUILD
#ifdef DEBUG_KERNEL_BUILD
	string get_error_string(int e) { return ""+e; }
#endif


int compute(double* T, double* Tnew, int size_i, int size_j, double k, double d, double c, double l, double delta_t, double max_time) {
	double *m_in, *m_out, *aux;
	m_in = T;
	m_out = Tnew;

	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_uint n_platforms = 0, n_devices = 0;
	cl_context_properties properties[3];
	cl_int err = CL_SUCCESS;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel1, kernel2;
	cl_mem bff1, bff2;

	chrono::high_resolution_clock::time_point t_start,t_end;
	chrono::duration<double> time;

	if (clGetPlatformIDs(1,&platform_id,&n_platforms) != CL_SUCCESS) {
		cerr << "error: no platform\n";
		return err;
	};

	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &n_devices) != CL_SUCCESS)
	{
		cerr << "error: no device\n";
		return err;
	};

	properties[0]= CL_CONTEXT_PLATFORM;
	properties[1]= (cl_context_properties) platform_id;
	properties[2]= 0;

	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);
	if (err != CL_SUCCESS){
		cerr << "error creating context\n";
		return err;
	};

	queue = clCreateCommandQueueWithProperties(context, device_id, (cl_command_queue_properties)0, &err);

	program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
	if (err != CL_SUCCESS){
		cerr << "error creating kernel program\n";
		return err;
	};

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS){
		cerr << "error building kernel program\n";

#ifdef DEBUG_KERNEL_BUILD
		size_t len;
		char buffer[204800];
		cl_build_status bldstatus;
		printf("\nError %d: Failed to build program executable [ %s ]\n",err,get_error_string(err));
		err = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_STATUS, sizeof(bldstatus), (void *)&bldstatus, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Status error %d: %s\n",err,get_error_string(err));
			return err;
		}
		if (bldstatus == CL_BUILD_SUCCESS) printf("Build Status: CL_BUILD_SUCCESS\n");
		if (bldstatus == CL_BUILD_NONE) printf("Build Status: CL_BUILD_NONE\n");
		if (bldstatus == CL_BUILD_ERROR) printf("Build Status: CL_BUILD_ERROR\n");
		if (bldstatus == CL_BUILD_IN_PROGRESS) printf("Build Status: CL_BUILD_IN_PROGRESS\n");
		err = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), buffer, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Options error %d: %s\n",err,get_error_string(err));
			return err;
		}
		printf("Build Options: %s\n", buffer);
		err = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		if (err != CL_SUCCESS)
		{
			printf("Build Log error %d: %s\n",err,get_error_string(err));
			return err;
		}
		printf("Build Log:\n%s\n", buffer);
#endif
		return err;
	};

	kernel1 = clCreateKernel(program, "gpu_compute", &err);
	kernel2 = clCreateKernel(program, "gpu_compute", &err);
	if (err != CL_SUCCESS){
		cerr << "error creating kernel executable\n";
		return err;
	};

	size_t global_pattern[] = {(size_t)size_i,(size_t)size_j,0};
	size_t group_pattern[] = {BLOCK,BLOCK,0};

	int steps = max_time/delta_t;
	double p = k/(d*c);
	double discr = delta_t/(l*l);

	bff1 = clCreateBuffer(context, /*CL_MEM_READ_ONLY*/   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		size_i * size_j * sizeof(cl_double), m_in, &err);
	bff2 = clCreateBuffer(context, /*CL_MEM_WRITE_ONLY*/ CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		size_i * size_j * sizeof(cl_double), m_out, &err);

	err |= clSetKernelArg(kernel1, 0, sizeof(bff1), &bff1);
	err |= clSetKernelArg(kernel1, 1, sizeof(bff2), &bff2);
	err |= clSetKernelArg(kernel1, 2, sizeof(size_i), &size_i);
	err |= clSetKernelArg(kernel1, 3, sizeof(size_j), &size_j);
	err |= clSetKernelArg(kernel1, 4, sizeof(p), &p);
	err |= clSetKernelArg(kernel1, 5, sizeof(discr), &discr);

	err |= clSetKernelArg(kernel2, 0, sizeof(bff2), &bff2);
	err |= clSetKernelArg(kernel2, 1, sizeof(bff1), &bff1);
	err |= clSetKernelArg(kernel2, 2, sizeof(size_i), &size_i);
	err |= clSetKernelArg(kernel2, 3, sizeof(size_j), &size_j);
	err |= clSetKernelArg(kernel2, 4, sizeof(p), &p);
	err |= clSetKernelArg(kernel2, 5, sizeof(discr), &discr);

	if (err != CL_SUCCESS){
		cerr << "error creating or passing parameters to kernel" << endl
			<< "size_i=" << size_i << endl
			<< "size_j=" << size_j << endl
			<< "p=" << p << endl
			<< "discr=" << discr << endl
			<< "steps=" << steps << endl;
		return err;
	};

// skip mem overhead
#ifdef SKIP_OVERHEAD
	t_start = chrono::high_resolution_clock::now();

	err = clEnqueueWriteBuffer(queue, bff1, CL_FALSE, 0, size_i * size_j * sizeof(cl_double), m_in, 0, NULL, NULL);
	// be sure the transfer is done before running the kernel
	err = clFinish(queue);
	if (err != CL_SUCCESS){
		cerr << "error passing input data\n";
		return err;
	};

	t_end = chrono::high_resolution_clock::now();


	time = (t_end - t_start);
	if (time.count() < min_mem_w_time)
		min_mem_w_time = time.count();
#endif

	///////////////////////////////////////////////
	t_start = chrono::high_resolution_clock::now();
	///////////////////////////////////////////////

	for(int step=0; step < steps; step++) {
		err = clEnqueueNDRangeKernel(queue, step % 2 == 0 ? kernel1 : kernel2, 2, NULL, global_pattern, group_pattern, 0, NULL, NULL);
		if (err != CL_SUCCESS){
			cerr << "error running kernel\n";
			return err;
		};
	}

#ifdef SKIP_OVERHEAD
	err = clFinish(queue);
#endif

	/////////////////////////////////////////////
	t_end = chrono::high_resolution_clock::now();
	/////////////////////////////////////////////

	time = (t_end - t_start)/steps;
	if (time.count() < min_exec_time)
		min_exec_time = time.count();

	t_start = chrono::high_resolution_clock::now();

	err = clEnqueueReadBuffer(queue, steps % 2 ? bff2 : bff1, CL_TRUE, 0,
		size_i * size_j * sizeof(cl_double), m_out, 0, NULL, NULL);
	if (err != CL_SUCCESS){
		cerr << "error getting results\n";
		return err;
	};

	t_end = chrono::high_resolution_clock::now();
	time = (t_end - t_start);
	if (time.count() < min_mem_r_time)
		min_mem_r_time = time.count();

	clReleaseMemObject(bff1);
	clReleaseMemObject(bff2);
	clReleaseKernel(kernel1);
	clReleaseKernel(kernel2);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;
}


int main(int argc, char** argv) {
	if(argc != 11)
		return 1;

	double slice_i, slice_j, k, d, c, l, delta_t, max_time;
	string filename_in, filename_out;

	slice_i = stod(argv[1]);	// slice size (rows)
	slice_j = stod(argv[2]);	// slice size (cols)
	k = stod(argv[3]);			// thermal conductivity
	d = stod(argv[4]);			// density
	c = stod(argv[5]);			// specific heat
	l = stod(argv[6]);			// discretization step
	delta_t = stod(argv[7]);	// duration
	max_time = stod(argv[8]);	// simulation time

	filename_in = argv[9];
	filename_out = argv[10];

	// normalization
	int size_i = slice_i/l;
	int size_j = slice_j/l;

	double *T, *Tnew;
	T = new double[size_i*size_j];
	Tnew = new double[size_i*size_j];

	for(int trial=0; trial<TRIALS; trial++) {
		// Initialize T
		ReadMatrix(T, filename_in, size_i, size_j);

		// Tnew has to be initialized too because its edges are not computed
		// 	(otherwise they would have the wrong value)
		ReadMatrix(Tnew, filename_in, size_i, size_j);

	//	PrintMatrix_Nice(T, size_i, size_j);

		int status = compute(T, Tnew, size_i, size_j, k, d, c, l, delta_t, max_time);
		if(status) {
			cerr << "exit code: " << status << endl;
			return status;
		}
	}

	int steps = max_time/delta_t;
//	PrintMatrix_Nice(Tnew, size_i, size_j);
	PrintMatrix(Tnew, size_i, size_j, filename_out);

	printf("%d %f %f %f\n", steps, min_exec_time, min_mem_w_time, min_mem_r_time);
	//cout << "Execution time: " << min_exec_time*1e6 << " usec per cycle" << endl;

	return 0;
}
