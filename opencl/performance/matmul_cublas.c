// inspired to https://github.com/sol-prog/cuda_cublas_curand_thrust
// http://solarianprogrammer.com/2012/05/31/matrix-multiplication-cuda-cublas-curand-thrust

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cublas_v2.h>

// compile with: nvcc -lcublas

// USAGE: matmul <smaller> <larger> <step>
// OUTPUT: PERFORMANCE IN GFLOPS

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000000 /* 5 seconds */

int main ( int argc, char **argv ) {

	int i,j,k,n,p,nmin,nmax,step;
	double *a,*b,*c;
	const double alf = 1;
	const double bet = 0;
	const double *alpha = &alf;
	const double *beta = &bet;
	cublasHandle_t handle;
	struct timespec tstart,t1,t2;
	double ttstart,tt1,tt2,diff,min;

	if (argc < 4) return 0;
	nmin = atoi(argv[1]);
	nmax = atoi(argv[2]);
	step = atoi(argv[3]);
	if (nmin < 1 || nmin > nmax || step < 1) return 0;

/* allocate the three matrices and align to cache lines */
	a = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	b = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	c = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	a = (double *)(((unsigned long)a+CACHELINE)&~(CACHELINE-1));
	b = (double *)(((unsigned long)b+CACHELINE)&~(CACHELINE-1));
	c = (double *)(((unsigned long)c+CACHELINE)&~(CACHELINE-1));

// Allocate 3 arrays on GPU
	double *d_a, *d_b, *d_c;
	cudaMalloc(&d_a,nmax*nmax*sizeof(double));
	cudaMalloc(&d_b,nmax*nmax*sizeof(double));
	cudaMalloc(&d_c,nmax*nmax*sizeof(double));

	cublasCreate(&handle);

/* initialize A and B */
	for (i=0;i<nmax;i++)
		for (j=0;j<nmax;j++) {
			a[i*nmax+j] = j;
			b[i*nmax+j] = i;
		}

/* performance evaluation for all matrix size from nmin to nmax */
	for (n=nmin; n<=nmax; n += step) {
		min = 1.0e100;
		clock_gettime(CLOCK_REALTIME,&tstart);
		ttstart = (double)tstart.tv_sec*1.0e9 + (double)tstart.tv_nsec;
/* for each matrix size, run at most TRIALS times */
		for (p=0; p<TRIALS; p++) {
			clock_gettime(CLOCK_REALTIME,&t1);

cudaMemcpy(d_a,a,n*n*sizeof(double),cudaMemcpyHostToDevice);
cudaMemcpy(d_b,b,n*n*sizeof(double),cudaMemcpyHostToDevice);
cublasDgemm(handle,CUBLAS_OP_N,CUBLAS_OP_N,n,n,n,alpha,d_a,n,d_b,n,beta,d_c,n);
cudaDeviceSynchronize();
cudaMemcpy(c,d_c,n*n*sizeof(double),cudaMemcpyDeviceToHost);

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

	cublasDestroy(handle);

	//Free GPU memory
	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_c);	

	return 0;

}
