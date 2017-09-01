/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */
#include <lisp.h>
#include "string.h"
#include "assert.h"

obj* quote(obj* o) {
  return o;
}

const obj* atom(obj* o) {
  return o->objtype == atom_obj ? t : empty;
}

const obj* eq(obj* o) {
  assert(o->objtype == list_obj);

  // todo

  obj* x = NULL;
  obj* y = NULL;

  if (atom(x) == t && atom(y) == t)
    return strcmp(x->p, y->p) ? t : empty;
  else return x->p == y->p ? t : empty;
}

obj* car(obj* o) {
  assert(o->objtype == list_obj);
  list_t* l = o->p;
  return l->car;
}

obj* cdr(obj* o) {
  assert(o->objtype == list_obj);
  list_t* l = o->p;
  return l->cdr;
}

obj* cons(obj* x, obj* y) {
  assert(y->objtype == list_obj);

  obj* o = calloc(1, sizeof(obj));
  o->objtype = list_obj;
  list_t* l = calloc(1, sizeof(list_t));
  l->car = x;
  l->cdr = y;
  o->p = l;
  return o;
}

obj* cond(obj* o) {
  assert(o->objtype == list_obj);
  list_t* l = o->p;

  while (true) {
    obj* pair_obj = l->car;
    assert(pair_obj->objtype == list_obj);
    list_t* pair = pair_obj->p;
    if (eval(pair->car) == t) return pair->cdr;

    if (l->cdr == NULL) return empty;
    assert(l->cdr->objtype == list_obj);
    l = l->cdr->p;
  }
}

obj* label(obj* o) {
  assert(o->objtype == list_obj);
  list_t* list = o->p;

  assert(cat)

  cons(a, env);
}

obj* lambda(obj* o) { return NULL; }