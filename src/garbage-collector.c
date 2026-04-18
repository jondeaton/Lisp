/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of the Lisp garbage collector.
 */

#include <lisp-objects.h>
#include <garbage-collector.h>
#include <list.h>

#include <stdlib.h>
#include <assert.h>

#define GC_INITIAL_CAPACITY 256

GarbageCollector *new_gc(void) {
  GarbageCollector *gc = malloc(sizeof(GarbageCollector));
  if (gc == NULL) return NULL;
  bool success = gc_init(gc);
  if (!success) {
    free(gc);
    return NULL;
  }
  return gc;
}

bool gc_init(GarbageCollector *gc) {
  gc->objects = malloc(sizeof(obj*) * GC_INITIAL_CAPACITY);
  if (gc->objects == NULL) return false;
  gc->count = 0;
  gc->capacity = GC_INITIAL_CAPACITY;
  gc->nil_cached = new_list_set(NULL, NULL);
  gc->nil_cached->reachable = true;
  gc->t_cached = new_atom("t");
  gc->t_cached->reachable = true;
  return true;
}

void gc_add(GarbageCollector *gc, obj *o) {
  if (o == NULL) return;
  if (gc->count == gc->capacity) {
    gc->capacity *= 2;
    gc->objects = realloc(gc->objects, sizeof(obj*) * gc->capacity);
    assert(gc->objects != NULL);
  }
  gc->objects[gc->count++] = o;
}

void gc_add_recursive(GarbageCollector *gc, obj *root) {
  if (root == NULL) return;
  if (root->reachable) return; // already tracked in this batch
  root->reachable = true;
  if (is_list(root)) {
    gc_add_recursive(gc, CAR(root));
    gc_add_recursive(gc, CDR(root));
  } else if (is_closure(root) || is_macro(root)) {
    gc_add_recursive(gc, PARAMETERS(root));
    gc_add_recursive(gc, PROCEDURE(root));
    gc_add_recursive(gc, CAPTURED(root));
  } else if (is_vector(root)) {
    for (int i = 0; i < VECTOR_LEN(root); i++)
      gc_add_recursive(gc, VECTOR(root)[i]);
  }
  gc_add(gc, root);
}

static void mark_recursive(obj *o) {
  if (o == NULL) return;
  if (o->reachable) return;
  o->reachable = true;
  if (is_list(o)) {
    mark_recursive(CAR(o));
    mark_recursive(CDR(o));
  } else if (is_closure(o) || is_macro(o)) {
    mark_recursive(PARAMETERS(o));
    mark_recursive(CAPTURED(o));
    mark_recursive(PROCEDURE(o));
  } else if (is_vector(o)) {
    for (int i = 0; i < VECTOR_LEN(o); i++)
      mark_recursive(VECTOR(o)[i]);
  }
}

void collect_garbage(GarbageCollector *gc, obj *env) {
  assert(gc != NULL);

  // Reset all reachable flags
  for (int i = 0; i < gc->count; i++) {
    gc->objects[i]->reachable = false;
  }

  // Mark from root
  mark_recursive(env);

  // Sweep: compact the array, freeing unreachable objects
  int write = 0;
  for (int read = 0; read < gc->count; read++) {
    if (gc->objects[read]->reachable) {
      gc->objects[write++] = gc->objects[read];
    } else {
      dispose(gc->objects[read]);
    }
  }
  gc->count = write;
}

void gc_dispose(GarbageCollector *gc) {
  assert(gc != NULL);
  for (int i = 0; i < gc->count; i++) {
    dispose(gc->objects[i]);
  }
  free(gc->objects);
  dispose(gc->nil_cached);
  dispose(gc->t_cached);
  gc->objects = NULL;
  gc->count = 0;
  gc->capacity = 0;
}
