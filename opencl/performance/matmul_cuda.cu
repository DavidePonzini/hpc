#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda_runtime.h>

__global__ void Muld(double *, double *, double *, int); 

// USAGE: matmul <smaller> <larger> <step>
// OUTPUT: PERFORMANCE IN GFLOPS

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000 /* 5 seconds */
#define BLOCK 32

int main ( int argc, char **argv ) {

	int i,j,n,p,nmin,nmax,step;
	double *a,*b,*c;
	struct timeval tstart,t1,t2;
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
	cudaMalloc((void **)&d_a,nmax*nmax*sizeof(double));
	cudaMalloc((void **)&d_b,nmax*nmax*sizeof(double));
	cudaMalloc((void **)&d_c,nmax*nmax*sizeof(double));


/* initialize A and B */
	for (i=0;i<nmax;i++)
		for (j=0;j<nmax;j++) {
			a[i*nmax+j] = j;
			b[i*nmax+j] = i;
		}

/* performance evaluation for all matrix size from nmin to nmax */
	for (n=nmin; n<=nmax; n += step) {

		for (;n % BLOCK; n++);

		min = 1.0e100;
		gettimeofday(&tstart,NULL);
		ttstart = (double)tstart.tv_sec*1.0e6 + (double)tstart.tv_usec;
/* for each matrix size, run at most TRIALS times */
		for (p=0; p<TRIALS; p++) {
			gettimeofday(&t1,NULL);

cudaMemcpy(d_a,a,n*n*sizeof(double),cudaMemcpyHostToDevice);
cudaMemcpy(d_b,b,n*n*sizeof(double),cudaMemcpyHostToDevice);
dim3 dimBlock(BLOCK, BLOCK); 
dim3 dimGrid(n / dimBlock.x, n / dimBlock.y); 
Muld<<<dimGrid,dimBlock>>>(d_a,d_b,d_c,n); 
cudaDeviceSynchronize();
cudaMemcpy(c,d_c,n*n*sizeof(double),cudaMemcpyDeviceToHost);

			gettimeofday(&t2,NULL);
			tt1 = (double)t1.tv_sec * 1.0e6 + (double)t1.tv_usec;
			tt2 = (double)t2.tv_sec * 1.0e6 + (double)t2.tv_usec;
			diff = tt2 - tt1;
/* take the best performance result */
			if (diff < min) min = diff;
/* ...at most TRIALS times no longer than MAXTIME */
			if ((tt2 - ttstart) > MAXTIME)
				break;
		}

/* print performance in GFLOPS */
		double dn = (double)n;
		fprintf(stderr,"%u %g\n",n,(2*dn*dn*dn-dn*dn)/(1000*min));

	}


	//Free GPU memory
	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_c);	

	return 0;

}

__global__ void Muld(double *A, double *B, double *C, int n) 
{ 
    // Block index 
    int bx = blockIdx.x; 
    int by = blockIdx.y; 
 
    // Thread index 
    int tx = threadIdx.x; 
    int ty = threadIdx.y; 
 
    // Index of the first sub-matrix of A processed by the block 
    int aBegin = n * BLOCK * by; 
 
    // Index of the last sub-matrix of A processed by the block 
    int aEnd   = aBegin + n - 1; 
 
    // Step size used to iterate through the sub-matrices of A 
    int aStep  = BLOCK; 
 
    // Index of the first sub-matrix of B processed by the block 
    int bBegin = BLOCK * bx; 
 
    // Step size used to iterate through the sub-matrices of B 
    int bStep  = BLOCK * n; 
 
    // The element of the block sub-matrix that is computed 
    // by the thread 
    double Csub = 0; 
 
    // Loop over all the sub-matrices of A and B required to 
    // compute the block sub-matrix 
    for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep) { 
 
        __shared__ double As[BLOCK][BLOCK]; 
        __shared__ double Bs[BLOCK][BLOCK]; 

        As[ty][tx] = A[a + n * ty + tx]; 
        Bs[ty][tx] = B[b + n * ty + tx]; 
        __syncthreads(); 
 
        for (int k = 0; k < BLOCK; ++k) 
            Csub += As[ty][k] * Bs[k][tx]; 
 
        __syncthreads(); 
    } 
 
    // Write the block sub-matrix to global memory; 
    // each thread writes one element 
    int c = n * BLOCK * by + BLOCK * bx; 
    C[c + n * ty + tx] = Csub; 
}
