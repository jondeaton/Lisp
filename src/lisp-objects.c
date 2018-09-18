/*
 * File: lisp-objects.c
 * --------------------
 * Presents the implementation of lisp object creation
 */

#include <lisp-objects.h>
#include <primitives.h>
#include <stack-trace.h>
#include <hash.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

obj* new_atom(atom_t name) {
  if (name == NULL) return NULL;
  size_t name_size = strlen(name);
  obj* o = malloc(sizeof(obj) + name_size + 1);
  MALLOC_CHECK(o);
  o->objtype = atom_obj;
  o->reachable = false;
  strcpy((char*) ATOM(o), name);
  return o;
}

obj* new_list() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  MALLOC_CHECK(o);
  o->objtype = list_obj;
  o->reachable = false;
  CAR(o) = NULL;
  CDR(o) = NULL;
  return o;
}

obj* new_closure() {
  obj* o = malloc(sizeof(obj) + sizeof(closure_t));
  MALLOC_CHECK(o);
  o->objtype = closure_obj;
  o->reachable = false;
  return o;
}

obj* copy_atom(const obj* o) {
  if (!is_atom(o)) return NULL;
  return new_atom(ATOM(o));
}

obj* copy_list(const obj *o) {
  obj* list_copy = new_list();
  memcpy(LIST(list_copy), LIST(o), sizeof(list_obj));
  return list_copy;
}

bool compare(const obj* a, const obj* b) {
  if (a == NULL || b == NULL) return a == b;
  if (a->objtype != b->objtype) return false;
  if (is_int(a)) return get_int(a) == get_int(b);
  if (is_float(a)) return get_float(a) == get_float(b);

  if (is_primitive(a))
    return *PRIMITIVE(a) == *PRIMITIVE(b);
  if (is_list(a))
    return memcmp(LIST(a), LIST(b), sizeof(list_t)) == 0;
  if (is_atom(a))
    return strcmp(ATOM(a), ATOM(b)) == 0;
  if (is_closure(a))
    return memcmp(CLOSURE(a), CLOSURE(b), sizeof(closure_t)) == 0;
  return false;
}

unsigned int atom_hash(const obj *o, size_t keysize UNUSED) {
  assert(o != NULL);
  assert(is_atom(o));
  return string_hash(ATOM(o), keysize);
}

void dispose(obj* o) {
  assert(o != NULL);
  free(o);
}

obj* new_int(int value) {
  obj* o = malloc(sizeof(obj) + sizeof(int));
  MALLOC_CHECK(o);
  o->objtype = int_obj;
  o->reachable = false;
  int *contents = (int*) CONTENTS(o);
  *contents = value;
  return o;
}

obj* new_float(float value) {
  obj* o = malloc(sizeof(obj) + sizeof(float));
  MALLOC_CHECK(o);
  o->objtype = float_obj;
  o->reachable = false;
  float *contents = (float*) CONTENTS(o);
  *contents = value;
  return o;
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
  if (is_float(o)) return *(float*) CONTENTS(o);

  LOG_ERROR("Object is not a number");
  return 0;
}

int get_int(const obj* o) {
  if (is_float(o)) return (int) get_float(o);
  if (is_int(o)) return *(int*) CONTENTS(o);
  LOG_ERROR("Object is not a number");
  return 0;
}
