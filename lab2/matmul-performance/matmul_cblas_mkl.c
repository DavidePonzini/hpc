#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mkl.h>

// compile with:  icc -O2 -mkl matmul_cblas_mkl.c

// USAGE: matmul <smaller> <larger> <step> <num_threads>
// OUTPUT: PERFORMANCE IN GFLOPS

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000000 /* 5 seconds */

int main ( int argc, char **argv ) {

	int i,j,k,n,p,nmin,nmax,step,nthreads;
	double *a,*b,*c;
	struct timespec tstart,t1,t2;
	double ttstart,tt1,tt2,diff,min;

	if (argc < 5) return 0;
	nmin = atoi(argv[1]);
	nmax = atoi(argv[2]);
	step = atoi(argv[3]);
	nthreads = atoi(argv[4]);
	if (nmin < 1 || nmin > nmax || step < 1 || nthreads < 1) return 0;

//          MKL_Set_Num_Threads(nthreads);
        mkl_set_num_threads(nthreads);

/* allocate the three matrices and align to cache lines */
	a = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	b = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	c = (double *)malloc(nmax*nmax*sizeof(double)+CACHELINE);
	a = (double *)(((unsigned long)a+CACHELINE)&~(CACHELINE-1));
	b = (double *)(((unsigned long)b+CACHELINE)&~(CACHELINE-1));
	c = (double *)(((unsigned long)c+CACHELINE)&~(CACHELINE-1));

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

                        cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,n,n,n,1,a,n,b,n,0,c,n);

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
	return 0;

}
