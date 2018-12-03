#include <iostream>
#include <string>

#include <ctime>
#include <chrono>

#include "file.h"

#include <CL/cl.h>

using namespace std;


#define  BLOCK 5


const char* source =
"#define BLOCK 5\n"
"kernel void g(global double* m_in, global double* m_out, int size_i, int size_j, double p, double discr, int steps) {"
"	int i = get_global_id(0);"
"	int j = get_global_id(1);"

"	int ii = get_local_id(0);"
"	int jj = get_local_id(1);"

"	local double l_in[(BLOCK+2)*(BLOCK+2)];"

"	global double* in = m_in;"
"	global double* out = m_out;"
"	global double* aux = NULL;"

"	for(int step=0; step<steps; step++) {"
"		l_in[(ii+1)*(BLOCK+2) + jj+1] = in[i*size_i+j];"		

		// copy block in local mem
"		if(!ii)"
"			l_in[jj+1] = in[(i-1)*size_i+j];"
"		if(!jj)"
"			l_in[(ii+1)*(BLOCK+2)] = in[i*size_i+j-1];"
"		if(ii == BLOCK-1)"
"			l_in[(BLOCK+1)*(BLOCK+2)+jj+1] = in[(i+1)*size_i+j];"
"		if(jj == BLOCK-1)"
"			l_in[(ii+1)*(BLOCK+2)+BLOCK+1] = in[i*size_i+j+1];"

"		barrier(CLK_LOCAL_MEM_FENCE);"

		// skip edges
"		if(i==0 || j == 0) return;"
"		if(i == size_i-1 || j == size_j-1) return;"

"		out[i*size_i + j] = l_in[(ii+1)*(BLOCK+2) + jj+1]*(1.0-4.0*discr*p) + discr*p*("
"				l_in[ii*(BLOCK+2) + jj+1] +"
"				l_in[(ii+2)*(BLOCK+2) + jj+1] +"
"				l_in[(ii+1)*(BLOCK+2) + jj] +"
"				l_in[(ii+1)*(BLOCK+2) + jj+2]);"

"		barrier(CLK_LOCAL_MEM_FENCE);"

"		aux=in;"
"		in=out;"
"		out=aux;"
"	}"
"}";


int f(double* T, double* Tnew, int size_i, int size_j, double k, double d, double c, double l, double delta_t, double max_time) {
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
	
	chrono::high_resolution_clock::time_point t_start,t_end;
    chrono::duration<double> exec_time;

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

	size_t global_pattern[] = {(size_t)size_i,(size_t)size_j,0};
	size_t group_pattern[] = {BLOCK,BLOCK,0};

	///////////////////////////////////////////////
    t_start = chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////
	
	//for(double t=0.0; t <= max_time; t += delta_t) {
		int steps = max_time/delta_t;
		double p = k/(d*c);
		double discr = delta_t/(l*l);

		in_bff = clCreateBuffer(context, /*CL_MEM_READ_ONLY*/   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			size_i * size_j * sizeof(cl_double), m_in, &err);
		out_bff = clCreateBuffer(context, /*CL_MEM_WRITE_ONLY*/ CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			size_i * size_j * sizeof(cl_double), m_out, &err);

		err |= clSetKernelArg(kernel, 0, sizeof(in_bff), &in_bff);
		err |= clSetKernelArg(kernel, 1, sizeof(out_bff), &out_bff);
		err |= clSetKernelArg(kernel, 2, sizeof(size_i), &size_i);
		err |= clSetKernelArg(kernel, 3, sizeof(size_j), &size_j);
		err |= clSetKernelArg(kernel, 4, sizeof(p), &p);
		err |= clSetKernelArg(kernel, 5, sizeof(discr), &discr);
		err |= clSetKernelArg(kernel, 6, sizeof(steps), &steps);
		
		if (err != CL_SUCCESS){
			cerr << "error creating or passing parameters to kernel\n";
			return err;
		};

		// skip mem overhead
#if 0
		err = clEnqueueWriteBuffer(queue, in_bff, CL_FALSE, 0, size_i * size_j * sizeof(cl_double), m_in, 0, NULL, NULL);
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

		err = clEnqueueReadBuffer(queue, out_bff, CL_TRUE, 0, size_i * size_j * sizeof(cl_double), m_out, 0, NULL, NULL);
		if (err != CL_SUCCESS){
			cerr << "error getting results\n";
			return err;
		};
		
		/////////////////////////////////////////////
		t_end = chrono::high_resolution_clock::now();
		/////////////////////////////////////////////

		clReleaseMemObject(in_bff);
		clReleaseMemObject(out_bff);

		aux = m_in;
		m_in = m_out;
		m_out = aux;
		
		exec_time = t_end - t_start;
		cout << "execution time: " << exec_time.count() * 1e6 << " usec\n";
//	}

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return 0;
}


int main(int argc, char** argv) {
	if(argc != 11)
		return 1;

	double slice, k, d, c, l, delta_t, max_time;
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
//	slice = l*size;

	double *T, *Tnew;
	T = new double[size_i*size_j];
	Tnew = new double[size_i*size_j];

	printf("t: %p\ntnew: %p\n\n", T, Tnew);

	ReadMatrix(T, filename_in, size_i, size_j);
	EmptyMatrix(Tnew, size_i, size_j);

	PrintMatrix_Nice(T, size_i, size_j);

	int status = f(T, Tnew, size_i, size_j, k, d, c, l, delta_t, max_time);
	if(status) {
		cerr << "exit code: " << status << endl;
		return status;
	}

	printf("t: %p\ntnew: %p\n\n", T, Tnew);

	double* result = ((int)(max_time/delta_t) % 2) ? Tnew : T;

	printf("res: %p\n", result);

	PrintMatrix_Nice(result, size_i, size_j);
	PrintMatrix(result, size_i, size_j, filename_out);

	return 0;
}
