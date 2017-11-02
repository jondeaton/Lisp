/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include "primitives.h"
#include "evaluator.h"
#include <string.h>
#include <assert.h>

// Static function declarations
static bool cmp(obj* x, obj* y);

obj t_atom = {
  .objtype = atom_obj
};

obj empty_atom = {
  .objtype = atom_obj
};

obj* t = &t_atom;
obj* empty = &empty_atom;

obj* quote(obj* o, obj* env) {
  if (o == NULL) return NULL;
  return get_list(o)->car;
}

obj* atom(obj* o, obj* env) {
  if (o == NULL) return NULL;
  return o->objtype == atom_obj ? t : empty;
}

obj* eq(obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  return cmp(car(o, env), cdr(o, env)) ? t : empty;
}

obj* car(obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(get_list(o)->car, env);
  return get_list(result)->car;
}

obj* cdr(obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(get_list(o)->car, env);
  return get_list(result)->cdr;
}

obj* cons(obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);

  obj* x = car(o, env);
  obj* y = cdr(o, env);

  obj* new_obj = calloc(1, sizeof(obj) + sizeof(list_t));
  new_obj->objtype = list_obj;
  list_t* l = get_list(new_obj);
  l->car = x;
  l->cdr = y;
  return new_obj;
}

obj* cond(obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = get_list(o);

  while (true) {
    obj* pair_obj = l->car;
    assert(pair_obj->objtype == list_obj);
    list_t* pair = get_list(pair_obj);
    if (eval(pair->car, env) == t) return pair->cdr;

    if (l->cdr == NULL) return empty;
    assert(l->cdr->objtype == list_obj);
    l = get_list(l->cdr);
  }
}

/**
 * Function: cmp
 * -------------
 * Deep comparison of two lisp objects
 * @param x : The first object to compare
 * @param y : The second object to compare
 * @return : True if the two objects are identical, false otherwise
 */
static bool cmp(obj* x, obj* y) {
  if (x->objtype != y->objtype) return false;
  if (x->objtype == atom_obj) return strcmp(get_atom(x), get_atom(y)) == 0;
  if (x->objtype == primitive_obj) return get_primitive(x) == get_primitive(y);
  if (x->objtype == list_obj)
    return cmp(get_list(x)->car, get_list(y)->car) && cmp(get_list(x)->cdr, get_list(y)->cdr);
  else return x == y;
}