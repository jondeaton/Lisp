/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of
 */

#include <garbage-collector.h>
#include <cvector.h>
#include <lisp-objects.h>

static void obj_cleanup(obj** op);

CVector* allocated; // Vector of allocated objects needing to be freed

void init_allocated() {
  size_t elemsz = sizeof(obj*);   // Vector stores pointers to objects needing to be free'd
  size_t capacity_hint = 0;       // no hint...
  CleanupElemFn cleanup_fn = (CleanupElemFn) &obj_cleanup;
  allocated = cvec_create(elemsz, capacity_hint, cleanup_fn);
}

void add_allocated(const obj* o) {
  cvec_append(allocated, &o);
}

void add_allocated_recursive(const obj* root) {
  if (root == NULL) return;
  if (is_list(root)) {
    add_allocated_recursive(list_of(root)->car);
    add_allocated_recursive(list_of(root)->cdr);
  } else if (is_closure(root)) {
    add_allocated_recursive(closure_of(root)->parameters);
    add_allocated_recursive(closure_of(root)->procedure);
    add_allocated_recursive(closure_of(root)->captured);
  }
  add_allocated(root);
}

void clear_allocated() {
  cvec_clear(allocated); // Free all the contents
}

void dispose_allocated() {
  cvec_dispose(allocated); // Destroy the vector
}

/**
 * Function: obj_cleanup
 * ---------------------
 * Cleanup an object given a pointer to a reference to the object. This
 * function was declared to use as the cleanup function for the CVector of
 * object references.
 * @param op: Pointer to a pointer to the object to dispose of
 */
static void obj_cleanup(obj** op) {
  dispose(*op);
}
