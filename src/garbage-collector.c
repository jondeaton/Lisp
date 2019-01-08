/*
 * File: garbage-collector.c
 * -------------------------
 * Presents the implementation of the Lisp garbage collector.
 */


#include <lisp-objects.h>
#include <garbage-collector.h>
#include <interpreter.h>

#include <stdlib.h>
#include <assert.h>

static void obj_cleanup(obj** op);

struct GarbageCollector *new_gc() {
  struct GarbageCollector *gc = (struct GarbageCollector*) malloc(sizeof(struct GarbageCollector));
  if (gc == NULL) return NULL;
  bool success = gc_init(gc);
  if (!success) {
    free(gc);
    return NULL;
  }
  return gc;
}

bool gc_init(struct GarbageCollector *gc) {
  size_t elemsz = sizeof(obj*);
  CleanupFn cleanup_fn = (CleanupFn) &obj_cleanup;
  return cvec_init(&gc->allocated, elemsz, 0, cleanup_fn);
}

void gc_add(struct GarbageCollector *gc, const obj *o) {
  cvec_append(&gc->allocated, &o);
}

void gc_add_recursive(struct GarbageCollector *gc, obj *root) {
  if (root == NULL) return;
  root->reachable = true;
  if (is_list(root)) {
    gc_add_recursive(gc, CAR(root));
    gc_add_recursive(gc, CDR(root));
  } else if (is_closure(root)) {
    gc_add_recursive(gc, PARAMETERS(root));
    gc_add_recursive(gc, PROCEDURE(root));
    // todo: ???
//    gc_add_recursive(gc, CAPTURED(root));
  }
  gc_add(gc, root);
}


static void mark_recursive(obj *o) {
  if (o == NULL) return;
  if (o->reachable) return;    // already seen

  // mark
  o->reachable = true;

  // recurse
  if (is_list(o)) {
    mark_recursive(CAR(o));
    mark_recursive(CDR(o));
  } else if (is_closure(o)) {
    mark_recursive(PARAMETERS(o));
    // todo: ??
//    mark_recursive(CAPTURED(o));
    mark_recursive(PROCEDURE(o));
  }
}

static bool is_reachable(const obj **objp) {
  assert(objp != NULL);
  const obj *o = *objp;
  assert(o != NULL);
  return o->reachable;
}

// marks all reachable objects within a variable scope
static void mark_reachable(struct Map *scope) {
  assert(scope != NULL);
  obj * const * varp;
  for_map_keys(scope, varp) {
    obj *var_name = *varp;
    obj * const * valuep = get_value(scope, var_name);
    mark_recursive(var_name);
    mark_recursive(*valuep);
  }
}

void collect_garbage(struct GarbageCollector *gc, struct environment *env) {
  assert(gc != NULL);

  // reset all the flags to not reached
  void *el;
  for_vector(&gc->allocated, el) {
    obj *o = *(obj **) el;
    if (o == NULL) continue;
    o->reachable = false;
  }

  // Mark all reachable objects in environment
  mark_reachable(&env->global_scope);
  struct Map *scope;
  for_vector(&env->stack, scope)
    mark_reachable(scope);

  // sweep
  cvec_filter(&gc->allocated, (PredicateFn) is_reachable);
}

void gc_dispose(struct GarbageCollector *gc) {
  assert(gc != NULL);
  cvec_dispose(&gc->allocated);
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
  assert(op != NULL);
  dispose(*op);
}
