#include <stdio.h>
#include <unistd.h>
#include <omp.h>

// SHARED vs. PRIVATE VARS
// SEE ROLE OF BARRIER
// TURN nthreads TO PRIVATE, SEE WRONG INITIALIZATION, USE firstprivate CLAUSE
// # OF THREADS DEFINED BY PROGRAMMER


int main()  {

int nthreads = 0, tid;

#pragma omp parallel num_threads(4) private(tid)
  {
    tid = omp_get_thread_num();
    printf("Hello World from thread = %d\n", tid);

    if (!tid)
      nthreads = omp_get_num_threads();

//#pragma omp barrier

    printf("Number of threads = %d\n", nthreads);

  }  // end of parallel section

  return 0;

}
