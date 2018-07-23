/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of the Lisp garbage collector.
 */


#include <lisp-objects.h>
#include <memory-manager.h>
#include "stdlib.h"

static void obj_cleanup(obj** op);

MemoryManager *new_mm() {
  MemoryManager *mm = (MemoryManager*) malloc(sizeof(MemoryManager));
  if (mm == NULL) return NULL;
  bool success = mm_init(mm);
  if (!success) {
    free(mm);
    return NULL;
  }
  return mm;
}

bool mm_init(MemoryManager *mm) {
  size_t elemsz = sizeof(obj*);
  CleanupElemFn cleanup_fn = (CleanupElemFn) &obj_cleanup;
#ifdef VECTOR
  return cvec_init(&mm->allocated, elemsz, 0, cleanup_fn);
#else
  return clist_init(&mm->allocated, elemsz, cleanup_fn);
#endif
}

void mm_add(MemoryManager *mm, const obj *o) {
#ifdef VECTOR
  cvec_append(&mm->allocated, &o);
#else
  clist_push_front(&mm->allocated, &o);
#endif
}

void mm_add_recursive(MemoryManager *gc, const obj *root) {
  if (root == NULL) return;
  if (is_list(root)) {
    mm_add_recursive(gc, CAR(root));
    mm_add_recursive(gc, CDR(root));
  } else if (is_closure(root)) {
    mm_add_recursive(gc, PARAMETERS(root));
    mm_add_recursive(gc, PROCEDURE(root));
    mm_add_recursive(gc, CAPTURED(root));
  }
  mm_add(gc, root);
}

void mm_clear(MemoryManager *mm) {
#ifdef VECTOR
  cvec_clear(&mm->allocated);
#else
  clist_clear(&mm->allocated); // Free all the contents
#endif
}

void mm_dispose(MemoryManager *mm) {
#ifdef VECTOR
  cvec_dispose(&mm->allocated);
#else
  clist_dispose(&mm->allocated); // Destroy the list of objects
#endif
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
