
#include <clist.h>
#include <string.h>

typedef struct CListNode Node;

struct CListNode {
  Node* next;
  Node* previous;
  void* data;
};

struct CListImplementation {
  Node* front;
  Node* back;
  int nelems;
  size_t elemsz;
  CleanupElemFn cleanup;
};

static inline Node* new_node(const CList* cl, const void* source);
static inline void delete_node(CList* cl, Node* node);

CList* clist_create(size_t elemsz, CleanupElemFn fn) {
  CList* cl = malloc(sizeof(CList));
  cl->front = NULL;
  cl->back = NULL;
  cl->nelems = 0;
  cl->elemsz = elemsz;
  cl->cleanup = fn;
  return cl;
}

void clist_clear(CList* cl) {
  while (cl->nelems) clist_pop_front(cl);
}

void clist_dispose(CList* cl) {
  clist_clear(cl);
  free(cl);
}

int clist_count(const CList* cl) {
  return cl->nelems;
}

void* clist_front(const CList* cl) {
  return cl->front->data;
}

void* clist_back(const CList* cl) {
  return cl->back->data;
}

void clist_push_front(CList* cl, const void* source) {
  Node* node = new_node(cl, source);
  node->next = cl->front;
  if (cl->front) cl->front->previous = node;
  cl->front = node;
  if (cl->back == NULL) cl->back = cl->front;
  cl->nelems++;
}

void clist_push_back(CList* cl, const void* source) {
  Node* node = new_node(cl, source);
  node->previous = cl->back;
  if (cl->back) cl->back->next = node;
  cl->back = node;
  if (cl->front == NULL) cl->front = cl->back;
  cl->nelems++;
}

void clist_pop_front(CList* cl) {
  if (cl->nelems == 0) return;

  Node* front = cl->front;
  cl->front = front->next;
  if (cl->front) cl->front->previous = NULL;

  delete_node(cl, front);
  cl->nelems--;
}

void clist_pop_back(CList* cl) {
  if (cl->nelems == 0) return;

  Node* back = cl->back;
  cl->back = back->previous;
  if (cl->back) cl->back->next = NULL;

  delete_node(cl, back);
  cl->nelems--;
}

clist_const_iterator clist_begin(const CList* cl) {
  return (clist_const_iterator) cl->front;
}

clist_const_iterator clist_end() {
  return NULL;
}

clist_const_iterator clist_next(clist_const_iterator it) {
  const Node* node = (const Node*) it;
  return (clist_const_iterator) node->next;
}

clist_const_iterator clist_prev(clist_const_iterator it) {
  const Node* node = (const Node*) it;
  return (clist_const_iterator) node->previous;
}

void clist_remove(CList* cl, clist_iterator it) {
  Node* node = (Node*) it;
  Node* prev = node->previous;
  Node* next = node->next;
  if (prev) prev->next = next;
  if (next) next->previous = prev;
  delete_node(cl, node);
  cl->nelems--;
}

void *clist_data(clist_const_iterator it) {
  const Node* node = (const Node*) it;
  return node->data;
}

static inline void delete_node(CList* cl, Node* node) {
  if (cl->cleanup) cl->cleanup(node->data);
  free(node->data);
  free(node);
}

static inline Node* new_node(const CList* cl, const void* source) {
  Node* node = malloc(sizeof(Node));
  node->next = NULL;
  node->previous = NULL;
  node->data = malloc(cl->elemsz);
  memcpy(node->data, source, cl->elemsz);
  return node;
}
