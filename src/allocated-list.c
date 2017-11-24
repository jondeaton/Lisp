/*
 * File: allocated-list.c
 * ----------------------
 * Presents the implementation of
 */

#include <allocated-list.h>
#include <cvector.h>

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

void clear_allocated() {
  cvec_clear(allocated); // Free all the contents
}

void dispose_allocated() {
  cvec_dispose(allocated); // Destroy the vector
}