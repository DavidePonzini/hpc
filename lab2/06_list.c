// MIGHT NOT WORK WITH GCC < 4.5.2 (openmp tasks not supported)

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NELEM 10000


typedef struct listelem {
  int val;
  struct listelem *next;
} listelem;


void visit (int tid, listelem *p) {
  printf("[%d] %d\n",tid,p->val);
  fflush(stdout);
// silly computation
  for (p->val *= 1000000;p->val;p->val = p->val-1);
  return;
}


int main()  {

  listelem *head, *tail, *newelem, *p;
  int i,tid;

// build the list

  head = tail = NULL;

  for (i=0; i<NELEM; i++) {
    newelem = malloc(sizeof(listelem));
    newelem->val = i;
    newelem->next = NULL;
    if (head == NULL)
      head = newelem;
    else
      tail->next = newelem;
    tail = newelem;
  }

// visit the list in parallel

#if 0
  #pragma omp parallel private(tid,p)
  {
    tid = omp_get_thread_num();
    printf("Hello from thread %d\n",tid);
    for (p = head; p != NULL; p = p->next)
      visit(tid,p);
  }
#endif

#if 0
  #pragma omp parallel private(tid,p)
  {
    tid = omp_get_thread_num();
    printf("Hello from thread %d\n",tid);
    #pragma omp master
    for (p = head; p != NULL; p = p->next)
      visit(tid,p);
  }
#endif

#if 1
  #pragma omp parallel private(tid,p)
  {
    #pragma omp master
    for (p = head; p != NULL; p = p->next)
      #pragma omp task
      {
        tid = omp_get_thread_num();
        visit(tid,p);
      }
  }
#endif

  return 0;

}
