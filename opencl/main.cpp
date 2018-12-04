#include <iostream>
#include <string>

#include <ctime>
#include <chrono>

#include "file.h"

using namespace std;


#ifndef STEPS
	#define STEPS 1
#endif
double min_exec_time = 1.0e100;


void compute(double* T, double* Tnew, int size_i, int size_j, double k, double d, double c, double l, double delta_t, double max_time) {
	double *m_in, *m_out, *aux;
	m_in = T;
	m_out = Tnew;

	chrono::high_resolution_clock::time_point t_start,t_end;
    chrono::duration<double> exec_time;
	
	double p = k/(d*c);
	double discr = delta_t/(l*l);
	
	///////////////////////////////////////////////
    t_start = chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////
	
	for(double t=0.0; t <= max_time; t += delta_t) {	
		for(int i=1; i<size_i-1; i++) {
			for(int j=1; j<size_j-1; j++) {
				m_out[i*size_j + j] = m_in[i*size_j + j]*(1.0-4.0*discr*p) + discr*p*(
					m_in[(i-1)*size_j + j] + m_in[(i+1)*size_j + j] + m_in[i*size_j + j-1] + m_in[i*size_j + j+1]);
			}
		}
		
		aux = m_in;
		m_in = m_out;
		m_out = aux;
	}
	
	/////////////////////////////////////////////
	t_end = chrono::high_resolution_clock::now();
	/////////////////////////////////////////////

	exec_time = (t_end - t_start)/(int)(max_time/delta_t);
	if (exec_time.count() < min_exec_time)
		min_exec_time = exec_time.count();
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
	for(int step=0; step<STEPS; step++) {
		T = new double[size_i*size_j];
		Tnew = new double[size_i*size_j];
		
		ReadMatrix(T, filename_in, size_i, size_j);
		ReadMatrix(Tnew, filename_in, size_i, size_j);
		
//		PrintMatrix_Nice(T, size_i, size_j);
		
		compute(T, Tnew, size_i, size_j, k, d, c, l, delta_t, max_time);
	}
	
	double* result = ((int)(max_time/delta_t) % 2) ? Tnew : T;
	
//	PrintMatrix_Nice(result, size_i, size_j);
	PrintMatrix(result, size_i, size_j, filename_out);
	
	cout << "Execution time: " << min_exec_time*1e6 << " usec per cycle" << endl;
	
	return 0;
}

