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
#include <stdlib.h>

const char* t_contents = "t";

// Static function declarations
static bool is_t(const obj* o);

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
  list_of(o)->car = NULL;
  list_of(o)->cdr = NULL;
  return o;
}

obj* quote(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  return list_of(o)->car;
}

obj* atom(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  obj* result = eval(list_of(o)->car, env);
  return is_empty(result) || result->objtype == atom_obj ? t() : empty();
}

obj* eq(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  obj* first_arg = list_of(o)->car;
  obj* second_arg = list_of(list_of(o)->cdr)->car;

  obj* x = eval(first_arg, env);
  obj* y = eval(second_arg, env);

  if (x->objtype != y->objtype) return empty();
  if (x->objtype == list_obj)
    return is_empty(x) && is_empty(y) ? t() : empty();

  return strcmp(atom_of(x), atom_of(y)) == 0 ? t() : empty();
}

obj* car(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(list_of(o)->car, env);
  return list_of(result)->car;
}

obj* cdr(const obj* o, obj* env) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  obj* result = eval(list_of(o)->car, env);
  return list_of(result)->cdr;
}

obj* cons(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  obj* x = list_of(o)->car;
  obj* y = list_of(list_of(o)->cdr)->car;

  obj* new_obj = empty();
  list_of(new_obj)->car = eval(x, env);
  list_of(new_obj)->cdr = eval(y, env);

  return new_obj;
}

obj* cond(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  obj* pair = list_of(o)->car;
  list_t* pl = list_of(pair);

  obj* predicate = eval(pl->car, env);
  if (is_t(predicate)) {
    obj* e = list_of(pl->cdr)->car;
    return eval(e, env);
  } else {
    if (pl->cdr == NULL) return empty();
    return cond(list_of(o)->cdr, env);
  }
}

obj* set(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  obj* new_link = empty();
  list_of(new_link)->car = list_of(env)->car;
  list_of(new_link)->cdr = list_of(env)->cdr;

  obj* first_arg = list_of(o)->car;
  obj* second_arg = list_of(list_of(o)->cdr)->car;

  obj* var = eval(first_arg, env);
  obj* value = eval(second_arg, env);

  obj* pair_second = empty();
  list_of(pair_second)->car = value;

  obj* pair_first = empty();
  list_of(pair_first)->car = var;
  list_of(pair_first)->cdr = pair_second;

  list_of(env)->car = pair_first;
  list_of(env)->cdr = new_link;
  return value;
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
  return strcmp(atom_of(o), t_contents) == 0;
}