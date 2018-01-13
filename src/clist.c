
#include <clist.h>
#include <string.h>
#include <assert.h>

typedef struct CListNode Node;

struct CListNode {
  Node* next;
  Node* previous;
};

struct CListImplementation {
  Node* front;
  Node* back;
  int nelems;
  size_t elemsz;
  CleanupElemFn cleanup;
};

static Node* node_at(const CList* cl, int i);
static inline Node* new_node(const CList* cl, const void* source);
static inline void delete_node(CList* cl, Node* node);
static inline void* data_of(const Node* node);

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
  if (!cl->front) return NULL;
  return data_of(cl->front);
}

void* clist_back(const CList* cl) {
  if (!cl->back) return NULL;
  return data_of(cl->back);
}

void clist_push_front(CList* cl, const void* source) {
  Node* node = new_node(cl, source);
  node->next = cl->front;
  if (cl->front) cl->front->previous = node;
  cl->front = node;
  if (!cl->back) cl->back = cl->front;
  cl->nelems++;
}

void clist_push_back(CList* cl, const void* source) {
  Node* node = new_node(cl, source);
  node->previous = cl->back;
  if (cl->back) cl->back->next = node;
  cl->back = node;
  if (!cl->front) cl->front = cl->back;
  cl->nelems++;
}

void clist_insert(CList* cl, const void* source, int i) {
  if (i > cl->nelems) return; // invalid index

  Node* node = new_node(cl, source);
  Node* previous = node_at(cl, i - 1);

  Node* next;
  if (previous) next = previous->next;
  else next = cl->front;

  if (previous) previous->next = node;
  else cl->front = node;

  if (next) next->previous = node;
  else cl->back = node;

  node->next = next;
  node->previous = previous;

  cl->nelems++;
}

void clist_erase(CList* cl, int i) {
  Node* node = node_at(cl, i);
  if (!node) return;
  if (node->previous) node->previous->next = node->next;
  if (node->next) node->next->previous = node->previous;
  cl->nelems--;
}

void clist_pop_front(CList* cl) {
  if (cl->nelems == 0) return;

  Node* front = cl->front;
  cl->front = front->next;
  if (cl->front) cl->front->previous = NULL;
  else cl->back = NULL;

  delete_node(cl, front);
  cl->nelems--;
}

void clist_pop_back(CList* cl) {
  if (cl->nelems == 0) return;

  Node* back = cl->back;
  cl->back = back->previous;
  if (cl->back) cl->back->next = NULL;
  else cl->front = NULL;

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
  return data_of(node);
}

static Node* node_at(const CList* cl, int i) {
  if (cl->nelems <= i) return NULL; // invalid index

  Node* node = cl->front;
  while (i > 0 && node) {
    i--;
    node = node->next;
  }
  return node;
}

static inline void delete_node(CList* cl, Node* node) {
  if (cl->cleanup) cl->cleanup(data_of(node));
  free(node);
}

static inline Node* new_node(const CList* cl, const void* source) {
  Node* node = malloc(sizeof(Node) + cl->elemsz);
  assert(node);
  node->next = NULL;
  node->previous = NULL;
  memcpy(data_of(node), source, cl->elemsz);
  return node;
}

static inline void* data_of(const Node* node) {
  return (char*) node + sizeof(Node);
}

