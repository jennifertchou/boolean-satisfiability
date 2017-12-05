#include <stdlib.h>
#include <stdbool.h>
#include "queues.h"

/* Aux structure of linked lists */
struct list_node {
  queue_elem data;
  struct list_node* next;
};
typedef struct list_node list;

/* will diverge if list is circular! */
bool is_queue_segment(list* start, list* end)
{
  list *p = start;
  while (p != end) {
    if (p == NULL) return false;
    p = p->next;
  }
  return true;
}

/* Queues */ 

struct queue_header {
  list *front;
  list *back;
};

bool is_queue (queue Q) {
  if (Q == NULL) return false;
  if (Q->front == NULL || Q->back == NULL) return false;
  if (!is_queue_segment(Q->front, Q->back)) return false;
  return true;
}

bool queue_empty(queue Q) {
  return Q->front == Q->back;
}

queue queue_new() {
  queue Q = malloc(sizeof(struct queue_header));
  list *p = malloc(sizeof(struct list_node));
  /* Dummy node: does not need to be initialized! */
  Q->front = p;
  Q->back = p;

  return Q;
}

void enq(queue Q, queue_elem e)
{
  Q->back->data = e;
  Q->back->next = malloc(sizeof(struct list_node));
  Q->back = Q->back->next;
}

queue_elem deq(queue Q)
{
  queue_elem e = Q->front->data;  /* save old queue element to return */
  list* q = Q->front;             /* save old list node to free */
  Q->front = Q->front->next;
  free(q);                      /* free old list node */

  return e;                     /* return old queue element */
}

void queue_free(queue Q, void (*elem_free)(queue_elem e))
{

  while (Q->front != Q->back) {
    list* p = Q->front;
    if (elem_free != NULL) (*elem_free)(p->data);
    Q->front = Q->front->next;
    free(p);
  }

  free(Q->front); /* == Q->back */
  free(Q);
}
