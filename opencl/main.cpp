#include <iostream>
#include <string>

#include "file.h"

using namespace std;


void f(double* T, double* Tnew, int size, double k, double d, double c, double l, double delta_t, double max_time) {
	double *m_in, *m_out, *aux;
	m_in = T;
	m_out = Tnew;
	
	for(double t=0.0; t <= max_time; t += delta_t) {
		double p = k/(d*c);
		double discr = delta_t/(l*l);
		
		//printf("discr: %f\tp: %f\n", discr, p);
		
		for(int i=1; i<size-1; i++) {
			for(int j=1; j<size-1; j++) {
				m_out[i*size + j] = m_in[i*size + j]*(1.0-4.0*discr*p) + discr*p*(
					m_in[(i-1)*size + j] + m_in[(i+1)*size + j] + m_in[i*size + j-1] + m_in[i*size + j+1]);
		/*		printf("out[%d, %d] = %f*(1-4*%f*%f) + %f*%f*(%f + %f + %f + %f = %f\n",
							 i,  j,   m_in[(i-1)*size + j], discr,p,discr,p,
							 m_in[(i-1)*size + j], m_in[(i+1)*size + j], m_in[i*size + j-1], m_in[i*size + j+1],
							 m_out[i*size+j]);
		*/
			}
		}
		
		aux = m_in;
		m_in = m_out;
		m_out = aux;
	}
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
	
	f(T, Tnew, size, k, d, c, l, delta_t, max_time);
	
	printf("t: %p\ntnew: %p\n\n", T, Tnew);
	
	double* result = ((int)(max_time/delta_t) % 2) ? Tnew : T;
	
	printf("res: %p\n", result);
	
	PrintMatrix_Nice(result, size);
	PrintMatrix(result, size, filename_out);
	
	return 0;
}