#include <iostream>
#include <ctime>
#include <chrono>

#include <omp.h>

using namespace std;

#define CACHELINE 64
#define TRIALS 30
#define MAXTIME 5.0 /* 5 seconds */


#ifdef DEBUG
void printMatrix(string name, double* mat, int row, int col) {
    cout<<"\n [" + name + "] \n";
    for(int i=0 ; i<=row-1 ; i++) {
        for(int j=0 ; j<=col-1 ; j++)
            cout<< mat[i*row+j]<<" ";
        cout<<endl;
    }
    cout<<endl;
}
#endif

int main ( int argc, char **argv ) {

    int i,j,k,n,p,nmin,nmax,step;
    double *a,*b,*c;
    chrono::high_resolution_clock::time_point tstart,t1,t2;
    chrono::duration<double> diff;
    double min;

    if (argc < 4) return 0;
    nmin = atoi(argv[1]);
    nmax = atoi(argv[2]);
    step = atoi(argv[3]);
    if (nmin < 1 || nmin > nmax || step < 1) return 0;

// allocate the three matrices and align to cache lines
    a = new double[nmax*nmax+CACHELINE/sizeof(double)];
    b = new double[nmax*nmax+CACHELINE/sizeof(double)];
    c = new double[nmax*nmax+CACHELINE/sizeof(double)];
    a = (double *)(((unsigned long)a+CACHELINE)&~(CACHELINE-1));
	b = (double *)(((unsigned long)b+CACHELINE)&~(CACHELINE-1));
    c = (double *)(((unsigned long)c+CACHELINE)&~(CACHELINE-1));

// initialize A and B
    for (i=0;i<nmax;i++)
        for (j=0;j<nmax;j++) {
            a[i*nmax+j] = j;
            b[i*nmax+j] = i;
    }
	
#ifdef DEBUG
	printMatrix("a", a, nmax, nmax);
	printMatrix("b", b, nmax, nmax);
#endif
	
// performance evaluation for all matrix size from nmin to nmax

    for (n=nmin; n<=nmax; n += step) {

        min = 1.0e100;
        tstart = chrono::high_resolution_clock::now();

#ifdef DEBUG
	#undef TRIALS
	#define TRIALS 1
#endif

// for each matrix size, run at most TRIALS times
        for (p=0; p<TRIALS; p++) {

            t1 = chrono::high_resolution_clock::now();

#pragma omp parallel for private (i,j,k) schedule (static)
            for (i=0;i<n;i++)
                for (k=0;k<n;k++)
                    for (j=0;j<n;j++) {
                        if (k==0)
							c[i*n+j] = 0;
                        
						c[i*n+j] += a[i*n+k] * b[k*n+j];
                	}
			
			t2 = chrono::high_resolution_clock::now();

			diff = t2 - t1;
			
#ifdef DEBUG
			printMatrix("c", c, nmax, nmax);
#endif

// take the best performance result
            if (diff.count() < min) min = diff.count();

// quit measurement if max time exceeded
            diff = chrono::duration_cast<chrono::duration<double>>(t2 - tstart);
            if (diff.count() > MAXTIME) break;

        }

#if 0
// print performance in seconds
        cout << n << " " << min << endl;
#else
// print performance in GFLOPS
        cout << n << " " << (2*(double)n*n*n - (double)n*n)/(min*1e9) << endl;
#endif
    }

    return 0;

}
