
#include <clist.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#define ASSERT_NOT_NULL(x) assert((x) != NULL);

typedef struct CListNode Node;


/**
 * @struct CListNode. A single element of a linked
 */
struct CListNode {
  Node* next;
  Node* previous;
  char data[];
};

struct CListImplementation {
  Node* front;
  Node* back;
  int nelems;
  size_t elem_size;
  CleanupElemFn cleanup;
};

static Node* node_at(const CList* cl, int i);
static inline Node *new_node(const void *data, size_t elem_size);
static inline void delete_node(CList* cl, Node* node);
static void *data_of(const Node *node);
static void *data_to_node(const void *data);

CList* clist_create(size_t elem_size, CleanupElemFn cleanupFn) {
  CList* cl = malloc(sizeof(CList));
  if (cl == NULL)
    return NULL;
  cl->front = NULL;
  cl->back = NULL;
  cl->nelems = 0;
  cl->elem_size = elem_size;
  cl->cleanup = cleanupFn;
  return cl;
}

void clist_clear(CList* cl) {
  ASSERT_NOT_NULL(cl);
  while (cl->nelems) clist_pop_front(cl);
}

void clist_dispose(CList* cl) {
  ASSERT_NOT_NULL(cl);
  clist_clear(cl);
  free(cl);
}

int clist_count(const CList* cl) {
  ASSERT_NOT_NULL(cl);
  return cl->nelems;
}

void *clist_front(const CList* cl) {
  ASSERT_NOT_NULL(cl);
  if (cl->front == NULL) return NULL;
  return data_of(cl->front);
}

void *clist_back(const CList* cl) {
  ASSERT_NOT_NULL(cl);
  if (cl->back == NULL) return NULL;
  return data_of(cl->back);
}

void clist_push_front(CList* cl, const void* data) {
  ASSERT_NOT_NULL(cl);
  ASSERT_NOT_NULL(data);
  Node* node = new_node(data, cl->elem_size);
  node->next = cl->front;
  if (cl->front) cl->front->previous = node;
  cl->front = node;
  if (!cl->back) cl->back = cl->front;
  cl->nelems++;
}

void clist_push_back(CList* cl, const void* data) {
  ASSERT_NOT_NULL(cl);
  ASSERT_NOT_NULL(data);
  Node* node = new_node(data, cl->elem_size);
  node->previous = cl->back;
  if (cl->back) cl->back->next = node;
  cl->back = node;
  if (!cl->front) cl->front = cl->back;
  cl->nelems++;
}

void clist_insert(CList* cl, const void* data, int index) {
  ASSERT_NOT_NULL(cl);
  ASSERT_NOT_NULL(data);
  assert(index >= 0);
  assert(index < cl->nelems);

  Node* node = new_node(data, cl->elem_size);
  if (node == NULL) return;

  Node *previous = index == 0 ? NULL : node_at(cl, index - 1);

  Node* next;
  if (previous == NULL) {
    next = cl->front;
    cl->front = node;
  } else {
    next = previous->next;
    previous->next = node;
  }

  if (next != NULL) next->previous = node;
  else cl->back = node;

  node->next = next;
  node->previous = previous;

  cl->nelems++;
}

void clist_erase(CList* cl, int index) {
  ASSERT_NOT_NULL(cl);
  Node* node = node_at(cl, index);
  if (!node) return;
  if (node->previous) node->previous->next = node->next;
  if (node->next) node->next->previous = node->previous;
  cl->nelems--;
}

void clist_pop_front(CList* cl) {
  ASSERT_NOT_NULL(cl);
  if (cl->nelems == 0) return;

  Node* front = cl->front;
  cl->front = front->next;
  if (cl->front) cl->front->previous = NULL;
  else cl->back = NULL;

  delete_node(cl, front);
  cl->nelems--;
}

void clist_pop_back(CList* cl) {
  ASSERT_NOT_NULL(cl);
  if (cl->nelems == 0) return;

  Node* back = cl->back;
  cl->back = back->previous;
  if (cl->back) cl->back->next = NULL;
  else cl->front = NULL;

  delete_node(cl, back);
  cl->nelems--;
}

void *clist_next(const void *data) {
  ASSERT_NOT_NULL(data);
  const Node *node = data_to_node(data);
  return data_of(node->next);
}

void *clist_prev(const void *data) {
  ASSERT_NOT_NULL(data);
  const Node *node = data_to_node(data);
  return data_of(node->previous);
}

void clist_remove(CList *cl, void *data) {
  ASSERT_NOT_NULL(cl);
  ASSERT_NOT_NULL(data);
  
  Node* node = data_to_node(data);
  Node* prev = node->previous;
  Node* next = node->next;
  if (prev) prev->next = next;
  if (next) next->previous = prev;

  delete_node(cl, node);
  cl->nelems--;
}

static Node* node_at(const CList* cl, int i) {
  ASSERT_NOT_NULL(cl);
  assert(i >= 0);
  assert(i < cl->nelems);

  Node* node = cl->front;
  while (i > 0 && node != NULL) {
    i--;
    node = node->next;
  }
  return node;
}

static inline void delete_node(CList* cl, Node* node) {
  ASSERT_NOT_NULL(cl);
  ASSERT_NOT_NULL(node);
  if (cl->cleanup != NULL)
    cl->cleanup(data_of(node));
  free(node);
}

/**
 * Function: new_node
 * ------------------
 * @param cl Pointer to a linked
 * @param data Pointer to data to be copied into the new node. If NULL
 * is passed then the data field of the node will be left uninitialized.
 * @return Pointer to a new node, if one was created, else NULL
 */
static inline Node *new_node(const void *data, size_t elem_size) {
  Node* node = malloc(sizeof(Node) + elem_size);
  if (node == NULL) return NULL;
  node->next = NULL;
  node->previous = NULL;

  // Leave data uninitialized if no data is given
  if (data != NULL)
    memcpy(data_of(node), data, elem_size);
  return node;
}

/**
 * Function: data_of
 * -----------------
 * Returns a pointer to the data stored in the node
 * @param node Pointer to a linked list node
 * @return Pointer to the start of the data stored in the node
 */
static void *data_of(const Node *node) {
  ASSERT_NOT_NULL(node);
  return (void *) &node->data;
}

/**
 * Function: data_to_node
 * ----------------------
 * Gives a pointer to the node that encapsulates the given data
 * @param data Pointer to the data field of some linked list node
 * @return Pointer to the beginning of the node data structure which
 * contains the given data.
 */
static void *data_to_node(const void *data) {
  return (char *) data - offsetof(Node, data);
}
