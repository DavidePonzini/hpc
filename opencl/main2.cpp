#include <iostream>
#include <string>

#include "file.h"

#include <CL/cl.h>

using namespace std;

#ifndef BLOCK
	#define  BLOCK 1
#endif


const char* source =
"#define BLOCK 5\n"
"kernel void g(global double* m_in, global double* m_out, int size, double p, double discr) {"
"	int i = get_global_id(0);"
"	int j = get_global_id(1);"

"	int ii = get_local_id(0);"
"	int jj = get_local_id(1);"

"	local double l_in[(BLOCK+2)*(BLOCK+2)];"

"	l_in[(ii+1)*(BLOCK+2) + jj+1] = m_in[i*size+j];"

"	if(!ii)"
"		l_in[jj+1] = m_in[(i-1)*size+j];"
"	if(!jj)"
"		l_in[(ii+1)*(BLOCK+2)] = m_in[i*size+j-1];"
"	if(ii == BLOCK-1)"
"		l_in[(BLOCK+1)*(BLOCK+2)+jj+1] = m_in[(i+1)*size+j];"
"	if(jj == BLOCK-1)"
"		l_in[(ii+1)*(BLOCK+2)+BLOCK+1] = m_in[i*size+j+1];"

"	barrier(CLK_LOCAL_MEM_FENCE);"

"	if(i==0 || j == 0) {"
//"		m_out[i*size + j] = 1111;"
"		return;"
"	}"
"	if(i == size-1 || j == size-1) {"
//"		m_out[i*size + j] = 2222;"
"		return;"
"	}"

"	m_out[i*size + j] = l_in[(ii+1)*(BLOCK+2) + jj+1]*(1.0-4.0*discr*p) + discr*p*("
"				l_in[ii*(BLOCK+2) + jj+1] +"
"				l_in[(ii+2)*(BLOCK+2) + jj+1] +"
"				l_in[(ii+1)*(BLOCK+2) + jj] +"
"				l_in[(ii+1)*(BLOCK+2) + jj+2]);"
"}";


int f(double* T, double* Tnew, int size, double k, double d, double c, double l, double delta_t, double max_time) {
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
	cl_kernel kernel;
	cl_mem in_bff, out_bff;

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
		return err;
	};

	kernel = clCreateKernel(program, "g", &err);
	if (err != CL_SUCCESS){
		cerr << "error creating kernel executable\n";
		return err;
	};

	size_t global_pattern[] = {(size_t)size,(size_t)size,0};
	size_t group_pattern[] = {BLOCK,BLOCK,0};

	for(double t=0.0; t <= max_time; t += delta_t) {
		double p = k/(d*c);
		double discr = delta_t/(l*l);

		in_bff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * size * sizeof(cl_double), m_in, &err);
		out_bff = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, size * size * sizeof(cl_double), m_out, &err);

		err |= clSetKernelArg(kernel, 0, sizeof(in_bff), &in_bff);
		err |= clSetKernelArg(kernel, 1, sizeof(out_bff), &out_bff);
		err |= clSetKernelArg(kernel, 2, sizeof(size), &size);
		err |= clSetKernelArg(kernel, 3, sizeof(p), &p);
		err |= clSetKernelArg(kernel, 4, sizeof(discr), &discr);
		if (err != CL_SUCCESS){
			cerr << "error creating or passing parameters to kernel\n";
			return err;
		};

		// skip mem overhead
#if 0
		err = clEnqueueWriteBuffer(queue, in_bff, CL_FALSE, 0, size * size * sizeof(cl_double), m_in, 0, NULL, NULL);
		// be sure the transfer is done before running the kernel
		err = clFinish(queue);
		if (err != CL_SUCCESS){
			cerr << "error passing input data\n";
			return err;
		};
#endif

		err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_pattern, group_pattern, 0, NULL, NULL);
		if (err != CL_SUCCESS){
			cerr << "error running kernel\n";
			return err;
		};

#if 0
		err = clFinish(queue);
#endif

		err = clEnqueueReadBuffer(queue, out_bff, CL_TRUE, 0, size * size * sizeof(cl_double), m_out, 0, NULL, NULL);
		if (err != CL_SUCCESS){
			cerr << "error getting results\n";
			return err;
		};

		clReleaseMemObject(in_bff);
		clReleaseMemObject(out_bff);

		aux = m_in;
		m_in = m_out;
		m_out = aux;
	}

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;
}


int main(int argc, char** argv) {
	if(argc != 10)
		return 1;

	double slice, k, d, c, l, delta_t, max_time;
	string filename_in, filename_out;

	slice = stod(argv[1]);		// slice size
	k = stod(argv[2]);			// thermal conductivity
	d = stod(argv[3]);			// density
	c = stod(argv[4]);			// specific heat
	l = stod(argv[5]);			// discretization step
	delta_t = stod(argv[6]);	// duration
	max_time = stod(argv[7]);	// simulation time

	filename_in = argv[8];
	filename_out = argv[9];

	// normalization
	int size = slice/l;
	slice = l*size;

	double *T, *Tnew;
	T = new double[size*size];
	Tnew = new double[size*size];

	printf("t: %p\ntnew: %p\n\n", T, Tnew);

	ReadMatrix(T, filename_in, size);
	ReadMatrix(Tnew, filename_in, size);

	PrintMatrix_Nice(T, size);

	int status = f(T, Tnew, size, k, d, c, l, delta_t, max_time);
	if(status) {
		cerr << "exit code: " << status << endl;
		return status;
	}

	printf("t: %p\ntnew: %p\n\n", T, Tnew);

	double* result = ((int)(max_time/delta_t) % 2) ? Tnew : T;

	printf("res: %p\n", result);

	PrintMatrix_Nice(result, size);
	PrintMatrix(result, size, filename_out);

	return 0;
}
