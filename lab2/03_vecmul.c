#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <omp.h>

// USAGE: vecmul <smaller> <larger> <step>
// OUTPUT: PERFORMANCE IN GFLOPS

// LOOK AT REDUCTION CLAUSE IN PARALLEL DIRECTIVE

// TRY COMPILING WITHOUT -fopenmp,
// THEN WITH -fopenmp BUT 1 THREAD, SEE PERFORMANCE DIFFERENCE

// TRY INCREASING NO. OF THREADS, SEE PERFORMANCE INCREASE AND SATURATION

// LAUNCH WITH PARAMETERS smaller=50000 larger=150000 step=10000,
// larger sizes will hit the RAM bottleneck and performance of parallel
// version will be similar to sequential.

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5000000000 /* 5 seconds */


int main ( int argc, char **argv ) {

    int i,n,p,nmin,nmax,step;
    double *a,*b, c;
    struct timespec tstart,t1,t2;
    double ttstart,tt1,tt2,diff,min;

    if (argc < 4) return 0;
    nmin = atoi(argv[1]);
    nmax = atoi(argv[2]);
    step = atoi(argv[3]);
    if (nmin < 1 || nmin > nmax || step < 1) return 0;

// allocate the two vectors and align to cache lines
    a = (double *)malloc(nmax*sizeof(double)+CACHELINE);
    b = (double *)malloc(nmax*sizeof(double)+CACHELINE);
    a = (double *)(((unsigned long)a+CACHELINE)&~(CACHELINE-1));
    b = (double *)(((unsigned long)b+CACHELINE)&~(CACHELINE-1));

// initialize the two vectors
    for (i=0;i<nmax;i++) {
        a[i] = 1;
        b[i] = 1;
    }

// performance evaluation for all vector size from nmin to nmax
    for (n=nmin; n<=nmax; n+=step) {

        min = 1.0e100;
        clock_gettime(CLOCK_REALTIME,&tstart);
        ttstart = (double)tstart.tv_sec*1.0e9 + (double)tstart.tv_nsec;
// for each vector size, run at most TRIALS times
        for (p=0; p<TRIALS; p++) {
            clock_gettime(CLOCK_REALTIME,&t1);

            c = 0;

//#pragma omp parallel private (i) reduction (+:c)
//#pragma omp for schedule (static)

// this below is equivalent to the two above that are commented out:

#pragma omp parallel for private (i) reduction (+:c) schedule (static)
            for (i=0; i<n; i++)
                c += a[i] * b[i];

            clock_gettime(CLOCK_REALTIME,&t2);
            tt1 = (double)t1.tv_sec * 1.0e9 + (double)t1.tv_nsec;
            tt2 = (double)t2.tv_sec * 1.0e9 + (double)t2.tv_nsec;
            diff = tt2 - tt1;
// take the best performance result
            if (diff < min) min = diff;
// ...at most TRIALS times no longer than MAXTIME
            if ((tt2 - ttstart) > MAXTIME)
                break;
        }

// print performance in GFLOPS.
// The result of vec x vec is printed too, otherwise the compiler could
// realize that the result is useless and might "optimize" by omitting
// the computation at all.

fprintf(stderr,"%u %g (%g)\n", n, (2*(double)n - 1)/(double)min,c);

    }

    return 0;

}
