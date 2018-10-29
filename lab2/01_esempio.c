// from the LLNL OpenMP tutorial

#include <stdio.h>
#include <omp.h>

// SHARED vs. PRIVATE VARS
// DEFAULT: HOW MANY THREADS ARE GENERATED?
// CHANGE DEFAULT NO. OF THREADS BY USING ENV. VARIABLE OMP_NUM_THREADS
// WHAT DO YOU READ FROM CMD "top" WHEN THE FOR() LOOP IS UNCOMMENTED?

int main()  {

int nthreads, ncpu, tid;

#pragma omp parallel private(tid)
  {
    ncpu = omp_get_num_procs();
    tid = omp_get_thread_num();
    printf("Hello World from thread #%d over %d CPUs \n", tid, ncpu);
//int k;
//for (k=100000; k; k = (k/2)*2);

    if (!tid)  {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }

  }  // end of parallel section

  return 0;

}
