/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of
 */

#include <garbage-collector.h>
#include <clist.h>
#include <lisp-objects.h>

static void obj_cleanup(obj** op);

struct GarbageCollectorImpl {
  CList* allocated; // List of allocated objects needing to be freed
};

GarbageCollector * gc_init() {
  GarbageCollector* gc = (GarbageCollector*) malloc(sizeof(GarbageCollector));
  size_t elemsz = sizeof(obj*); // Vector stores pointers to objects needing to be free'd
  CleanupElemFn cleanup_fn = (CleanupElemFn) &obj_cleanup;
  gc->allocated = clist_create(elemsz, cleanup_fn);
  return gc;
}

void gc_add(GarbageCollector *gc, const obj *o) {
  clist_push_front(gc->allocated, &o);
}

void gc_add_recursive(GarbageCollector *gc, const obj *root) {
  if (root == NULL) return;
  if (LIST(root)) {
    gc_add_recursive(gc, LIST(root)->car);
    gc_add_recursive(gc, LIST(root)->cdr);
  } else if (CLOSURE(root)) {
    gc_add_recursive(gc, CLOSURE(root)->parameters);
    gc_add_recursive(gc, CLOSURE(root)->procedure);
    gc_add_recursive(gc, CLOSURE(root)->captured);
  }
  gc_add(gc, root);
}

void gc_clear(GarbageCollector *gc) {
  clist_clear(gc->allocated); // Free all the contents
}

void gc_dispose(GarbageCollector *gc) {
  clist_dispose(gc->allocated); // Destroy the list of objects
  free(gc);
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
