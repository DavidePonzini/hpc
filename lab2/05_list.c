#include <stdio.h>
#include <stdlib.h>

#define NELEM 10000


typedef struct listelem {
  int val;
  struct listelem *next;
} listelem;


void visit (listelem *p) {
  printf("%d\n",p->val);
  fflush(stdout);
// silly computation
  for (p->val *= 1000000;p->val;p->val = p->val-1);
  return;
}


int main()  {

  listelem *head, *tail, *newelem, *p;
  int i;

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

// visit the list
  for (p = head; p != NULL; p = p->next)
    visit(p);
    
  return 0;

}
