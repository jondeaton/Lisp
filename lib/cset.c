/**
 * @file cset.c
 *
 */

#include <cset.h>

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

enum Direction { left = 0, right = 1 };

struct CSetImplementation {
  struct Node *root;
  size_t data_size;
  CMapCmpFn cmp;
  CleanupFn cleanup;
};

struct Node {
  struct Node *left;
  struct Node *right;
  int height;
  int nright;
  int nleft;
  uint8_t data[];
};

// static function declarations
static struct Node *new_node(const void *data, size_t data_size);
static const struct Node *lookup(const CSet *set, const struct Node *node, const void *data);
static inline void update_height(struct Node *node);
static inline struct Node *child(const struct Node *node, enum Direction dir);
static struct Node *insert_at(CSet *set, struct Node *node, const void *data);
static struct Node *remove_at(CSet *set, struct Node *node, const void *data, struct Node **removed);
static struct Node *balance(struct Node *node);
static struct Node *rotate(struct Node *root, enum Direction dir);
static inline void dispose_node(const CSet *set, struct Node *node, bool recursive);
static struct Node *remove_node(CSet *set, struct Node *node);
static struct Node *extrema(struct Node *node, enum Direction dir);
static inline void assign_child(struct Node *node, struct Node *new_child, enum Direction dir);
static int get_balance(const struct Node * node);
static inline struct Node **child_ref(const struct Node *node, enum Direction dir);
static inline void update_count(struct Node *node, enum Direction dir);
static inline int num_ancestors(const struct Node *node, enum Direction dir);
static inline int get_size(const struct Node *node);
static inline enum Direction opposite(enum Direction dir);
static inline enum Direction get_direction(int comparison);

CSet *new_set(size_t data_size, CMapCmpFn cmp, CleanupFn cleanup) {
  assert(cmp != NULL);
  CSet *set = malloc(sizeof(CSet));
  if (set == NULL) return NULL;
  set->root = NULL;
  set->data_size = data_size;
  set->cmp = cmp;
  set->cleanup = cleanup;
  return set;
}

int set_size(const CSet *set) {
  assert(set != NULL);
  return get_size(set->root);
}

void set_insert(CSet *set, const void *data) {
  assert(set != NULL);
  assert(data != NULL);
  set->root = insert_at(set, set->root, data);
}

void *set_lookup(const CSet *set, const void *data) {
  assert(set != NULL);
  assert(data != NULL);
  const struct Node *node = lookup(set, set->root, data);
  if (node == NULL) return NULL;
  return (void *) node->data;
}

int set_rank(CSet *set, const void *data) {
  assert(set != NULL);
  assert(data != NULL);

  struct Node *node = set->root;
  int rank = 0;
  while (true) {
    if (node == NULL) return CSET_ERROR;
    int comparison = set->cmp(node->data, data, set->data_size);
    if (comparison == 0) return rank + node->nleft;
    if (comparison < 0) {
      rank += 1 + node->nleft;
      node = node->right;
    } else
      node = node->left;
  }
  return CSET_ERROR;
}

void set_remove(CSet *set, const void *data) {
  assert(set != NULL);
  assert(data != NULL);
  struct Node *removed;
  set->root = remove_at(set, set->root, data, &removed);
  if (removed != NULL)
    dispose_node(set, removed, false);
}

void set_clear(CSet *set) {
  assert(set != NULL);
  dispose_node(set, set->root, true);
  set->root = NULL;
}

void set_dispose(CSet *set) {
  assert(set != NULL);
  set_clear(set);
  free(set);
}

static struct Node *new_node(const void *data, size_t data_size) {
  assert(data != NULL);
  struct Node *node = malloc(sizeof(struct Node) + data_size);
  if (node == NULL) return NULL; // allocation failure
  node->left = NULL;
  node->right = NULL;
  node->height = 0;
  node->nleft = 0;
  node->nright = 0;
  memcpy(node->data, data, data_size);
  return node;
}

static const struct Node *lookup(const CSet *set, const struct Node *node, const void *data) {
  assert(set != NULL);
  assert(data != NULL);
  if (node == NULL) return NULL;
  int cmp = set->cmp(node->data, data, set->data_size);
  if (cmp == 0) return node;
  enum Direction dir = get_direction(cmp);
  return lookup(set, child(node, dir), data); // tail recursion! should be optimized
}

static struct Node *insert_at(CSet *set, struct Node *node, const void *data) {
  assert(set != NULL);
  if (node == NULL)
    return new_node(data, set->data_size);

  int comparison = set->cmp(node->data, data, set->data_size);
  if (comparison == 0) return node; // already there!

  enum Direction dir = get_direction(comparison);
  struct Node *new_child = insert_at(set, child(node, dir), data);
  if (new_child == NULL) return NULL; // insertion failure

  assign_child(node, new_child, dir);
  return balance(node);
}

static struct Node *remove_at(CSet *set, struct Node *node, const void *data, struct Node **removed) {
  assert(set != NULL);
  assert(data != NULL);
  if (node == NULL) {
    if (removed != NULL) *removed = NULL;
    return NULL; // not found in set
  }

  int comparison = set->cmp(node->data, data, set->data_size);
  if (comparison == 0) {
    if (removed != NULL) *removed = node;
    return remove_node(set, node);
  }

  enum Direction dir = get_direction(comparison);
  struct Node *new_child = remove_at(set, child(node, dir), data, removed);
  assign_child(node, new_child, dir);
  return balance(node);
}

static struct Node *remove_node(CSet *set, struct Node *node) {
  assert(set != NULL);
  assert(node != NULL);

  if (node->left == NULL) return node->right;
  if (node->right == NULL) return node->left;

  // take the highest node in the left sub-tree and make that the new root of this subtree
  struct Node *next = extrema(node->left, right);

  struct Node *new_root;
  struct Node *new_left = remove_at(set, node->left, next->data, &new_root);

  assign_child(new_root, node->right, right);
  assign_child(new_root, new_left, left);

  return balance(new_root);
}

static struct Node *balance(struct Node *node) {
  assert(node != NULL);
  int bal = get_balance(node); // left height minus right height
  if (bal >= -1 && bal <= 1) return node;

  enum Direction dir = bal > 1 ? left : right;

  struct Node **childp = child_ref(node, dir);
  int sub_bal = get_balance(*childp);

  if (dir == left) {
    if (sub_bal <= -1)
      *childp = rotate(*childp, dir);
  } else {
    if (sub_bal >= 1)
      *childp = rotate(*childp, dir);
  }
  return balance(rotate(node, opposite(dir)));
}

static int get_balance(const struct Node * node) {
  if (node == NULL) return 0;
  int l = node->left == NULL ? -1 : node->left->height;
  int r = node->right == NULL ? -1 : node->right->height;
  return l - r;
}

static struct Node *rotate(struct Node *root, enum Direction dir) {
  assert(root != NULL);

  struct Node *new_root = child(root, opposite(dir));
  if (new_root == NULL) return root; // nothing to rotate onto

  struct Node *transplant = child(child(root, opposite(dir)), dir);
  assign_child(root, transplant, opposite(dir));
  assign_child(new_root, root, dir);
  return new_root;
}

static inline void dispose_node(const CSet *set, struct Node *node, bool recursive) {
  assert(set != NULL);
  if (node == NULL) return;
  if (recursive) {
    dispose_node(set, node->left, recursive);
    dispose_node(set, node->right, recursive);
  }

  if (set->cleanup != NULL)
    set->cleanup(&node->data);
  free(node);
}

static inline void assign_child(struct Node *node, struct Node *new_child, enum Direction dir) {
  assert(node != NULL);
  *child_ref(node, dir) = new_child;
  update_count(node, dir);
  update_height(node);
}

static inline struct Node *child(const struct Node *node, enum Direction dir) {
  assert(node != NULL);
  return *child_ref(node, dir);
}

static inline struct Node **child_ref(const struct Node *node, enum Direction dir) {
  assert(node != NULL);
  return (struct Node **) (dir == left ? &node->left : &node->right);
}

static inline void update_height(struct Node *node) {
  if (node == NULL) return;
  int left_height = node->left == NULL ? -1 : node->left->height;
  int right_height = node->right == NULL ? -1 : node->right->height;
  node->height = 1 + max(left_height, right_height);
}

static struct Node *extrema(struct Node *node, enum Direction dir) {
  if (node == NULL) return NULL;
  if (child(node, dir) == NULL) return node;
  return extrema(child(node, dir), dir);
}

static inline void update_count(struct Node *node, enum Direction dir) {
  assert(node != NULL);
  int count = get_size(child(node, dir));
  if (dir == left) node->nleft = count;
  else node->nright = count;
}

static inline int num_ancestors(const struct Node *node, enum Direction dir) {
  assert(node != NULL);
  return dir == left ? node->nleft : node->nright;
}

static inline int get_size(const struct Node *node) {
  if (node == NULL) return 0;
  return 1 + num_ancestors(node, left) + num_ancestors(node, right);
}

static inline enum Direction opposite(enum Direction dir) {
  return dir == left ? right : left;
}

static inline enum Direction get_direction(int comparison) {
  return comparison > 0 ? left : right;
}
