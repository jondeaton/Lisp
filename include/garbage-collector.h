/*
 * File: garbage-collector.h
 * -------------------------
 * Presents the interface of the garbage collector for the Lisp interpreter.
 * Tracks all allocated objects and uses mark-and-sweep to free unreachable ones.
 */

#ifndef _LISP_MEMORY_MANAGER_H
#define _LISP_MEMORY_MANAGER_H

#include "lisp-objects.h"

typedef struct GarbageCollector {
  obj **objects;
  int count;
  int capacity;
} GarbageCollector;

GarbageCollector *new_gc(void);
bool gc_init(GarbageCollector *gc);
void gc_add(GarbageCollector *gc, obj *o);
void gc_add_recursive(GarbageCollector *gc, obj *root);
void collect_garbage(GarbageCollector *gc, obj *env);
void gc_dispose(GarbageCollector *gc);

#endif //_LISP_MEMORY_MANAGER_H
