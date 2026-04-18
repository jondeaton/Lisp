/*
 * File: lisp-objects.c
 * --------------------
 * Presents the implementation of lisp object creation
 */

#include <lisp-objects.h>
#include <stack-trace.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

obj* new_atom(atom_t name) {
  if (name == NULL) return NULL;
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = atom_obj;
  o->reachable = false;
  o->atom = strdup(name);
  return o;
}

obj* new_list(void) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = list_obj;
  o->reachable = false;
  CAR(o) = NULL;
  CDR(o) = NULL;
  return o;
}

obj* new_closure(void) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = closure_obj;
  o->reachable = false;
  PARAMETERS(o) = NULL;
  PROCEDURE(o) = NULL;
  CAPTURED(o) = NULL;
  NARGS(o) = 0;
  return o;
}

obj* new_int(int value) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = int_obj;
  o->reachable = false;
  o->intval = value;
  return o;
}

obj* new_float(float value) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = float_obj;
  o->reachable = false;
  o->floatval = value;
  return o;
}

bool compare(const obj* a, const obj* b) {
  if (a == NULL || b == NULL) return a == b;
  if (a->objtype != b->objtype) return false;
  if (is_int(a)) return a->intval == b->intval;
  if (is_float(a)) return a->floatval == b->floatval;
  if (is_primitive(a)) return a->primitive == b->primitive;
  if (is_atom(a)) return strcmp(a->atom, b->atom) == 0;
  if (is_list(a)) return CAR(a) == CAR(b) && CDR(a) == CDR(b);
  if (is_closure(a))
    return PARAMETERS(a) == PARAMETERS(b) &&
           PROCEDURE(a) == PROCEDURE(b) &&
           CAPTURED(a) == CAPTURED(b);
  return false;
}

void dispose(obj* o) {
  assert(o != NULL);
  if (is_atom(o)) free(o->atom);
  free(o);
}

bool is_atom(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == atom_obj;
}

bool is_primitive(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == primitive_obj;
}

bool is_list(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == list_obj;
}

bool is_closure(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == closure_obj;
}

bool is_int(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == int_obj;
}

bool is_float(const obj* o) {
  if (o == NULL) return false;
  return o->objtype == float_obj;
}

bool is_number(const obj* o) {
  if (o == NULL) return false;
  return is_float(o) || is_int(o);
}

bool is_t(const obj* o) {
  if (o == NULL) return false;
  if (!is_atom(o)) return false;
  return strcmp(ATOM(o), "t") == 0 || strcmp(ATOM(o), "true") == 0;
}

float get_float(const obj* o) {
  if (is_int(o)) return (float) get_int(o);
  if (is_float(o)) return o->floatval;
  LOG_ERROR("Object is not a number");
  return 0;
}

int get_int(const obj* o) {
  if (is_float(o)) return (int) get_float(o);
  if (is_int(o)) return o->intval;
  LOG_ERROR("Object is not a number");
  return 0;
}
