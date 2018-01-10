/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of
 */

#include <garbage-collector.h>
#include <clist.h>
#include <lisp-objects.h>

static void obj_cleanup(obj** op);

CList* allocated; // List of allocated objects needing to be freed

void init_allocated() {
  size_t elemsz = sizeof(obj*);   // Vector stores pointers to objects needing to be free'd
  CleanupElemFn cleanup_fn = (CleanupElemFn) &obj_cleanup;
  allocated = clist_create(elemsz, cleanup_fn);
}

void add_allocated(const obj* o) {
  clist_push_front(allocated, &o);
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
  clist_clear(allocated); // Free all the contents
}

void dispose_allocated() {
  clist_dispose(allocated); // Destroy the list of objects
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
