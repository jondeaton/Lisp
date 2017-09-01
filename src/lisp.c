/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include "lisp.h"
#include <string.h>
#include <assert.h>

obj t_atom = {
  .objtype = atom_obj,
  .p = "t"
};

obj empty_atom = {
  .objtype = atom_obj,
  .p = "()"
};

obj* t = &t_atom;
obj* empty = &empty_atom;


// Reduce the list
obj* eval(obj* o) {
  if (o == NULL) return NULL;
  // TODO
  return NULL;
};

obj* quote(obj* o) {
  return o;
}

obj* atom(obj* o) {
  return o->objtype == atom_obj ? t : empty;
}

obj* eq(obj* o) {
  assert(o->objtype == list_obj);

  obj* x = car(o);
  obj* y = cdr(o);

  if (atom(x) == t && atom(y) == t)
    return strcmp(x->p, y->p) ? t : empty;
  else return x->p == y->p ? t : empty;
}

obj* car(obj* o) {
  assert(o->objtype == list_obj);
  list_t* l = o->p;
  return eval(l->car);
}

obj* cdr(obj* o) {
  assert(o->objtype == list_obj);
  list_t* l = o->p;
  return eval(l->cdr);
}

obj* cons(obj* o) {
  assert(o->objtype == list_obj);

  obj* x = car(o);
  obj* y = cdr(o);

  obj* new_obj = calloc(1, sizeof(obj));
  new_obj->objtype = list_obj;
  list_t* l = calloc(1, sizeof(list_t));
  l->car = x;
  l->cdr = y;
  new_obj->p = l;
  return new_obj;
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
  return NULL;
}

obj* lambda(obj* o) {
  // TODO
  return NULL;
}