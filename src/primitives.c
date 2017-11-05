/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include "primitives.h"
#include "evaluator.h"
#include "list.h"
#include <string.h>
#include <assert.h>
#include <list.h>

const char* t_contents = "t";

// Static function declarations
static bool deep_compare(obj* x, obj* y);
static bool is_t(const obj* o);
static bool is_empty(const obj* o);

// Truth atom
obj* t() {
  obj* o = malloc(sizeof(obj) + strlen(t_contents) + 1);
  strcpy((char*) o + sizeof(obj), t_contents);
  o->objtype = atom_obj;
  return o;
}

// Empty list
obj* empty() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  o->objtype = list_obj;
  get_list(o)->car = NULL;
  get_list(o)->cdr = NULL;
  return o;
}

obj* quote(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  return get_list(o)->car;
}

obj* atom(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  obj* result = eval(o, env);
  return is_empty(result) || result->objtype == atom_obj ? t() : empty();
}

obj* eq(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  return deep_compare(car(o, env), cdr(o, env)) ? t() : empty();
}

obj* car(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(get_list(o)->car, env);
  return get_list(result)->car;
}

obj* cdr(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(get_list(o)->car, env);
  return get_list(result)->cdr;
}

obj* cons(const obj* o, obj* env) {
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

obj* cond(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = get_list(o);

  while (true) {
    obj* pair_obj = l->car;
    assert(pair_obj->objtype == list_obj);
    list_t* pair = get_list(pair_obj);

    obj* predicate = eval(pair->car, env);
    if (is_t(predicate)) return pair->cdr;

    if (l->cdr == NULL) return empty();
    assert(l->cdr->objtype == list_obj);
    l = get_list(l->cdr);
  }
}

obj* set(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  return NULL;
  // todo
}

/**
 * Function: deep_compare
 * ----------------------
 * Deep comparison of two lisp objects
 * @param x: The first object to compare
 * @param y: The second object to compare
 * @return: True if the two objects are identical, false otherwise
 */
static bool deep_compare(obj* x, obj* y) {
  if (x->objtype != y->objtype) return false;
  if (x->objtype == atom_obj) return strcmp(get_atom(x), get_atom(y)) == 0;
  if (x->objtype == primitive_obj) return get_primitive(x) == get_primitive(y);

  // List: cars must match and cdrs must match
  if (x->objtype == list_obj)
    return deep_compare(get_list(x)->car, get_list(y)->car) && deep_compare(get_list(x)->cdr, get_list(y)->cdr);
  else return x == y;
}

/**
 * Function: is_t
 * --------------
 * Determines if a lisp object is the truth atom
 * @param o: A lisp object to determine if it is the t atom
 * @return: True if it is the truth atom, false otherwise
 */
static bool is_t(const obj* o) {
  if (o == NULL) return false;
  if (o->objtype != atom_obj) return false;
  return strcmp(get_atom(o), t_contents) == 0;
}

/**
 * Function: is_empty
 * ------------------
 * @param o: A lisp object to determine if it is the empty list
 * @return: True if the object is the empty list, false otherwise
 */
static bool is_empty(const obj* o) {
  if (o == NULL) return false;
  if (o->objtype != list_obj) return false;
  return get_list(o)->car == NULL && get_list(o)->cdr == NULL;
}