/*
 * File: lisp-objects.c
 * --------------------
 * Presents the implementation of lisp object creation
 */

#include <lisp-objects.h>
#include <primitives.h>
#include <stack-trace.h>
#include <stdlib.h>
#include <string.h>

obj* new_atom(atom_t name) {
  if (name == NULL) return NULL;
  size_t name_size = strlen(name);
  obj* o = malloc(sizeof(obj) + name_size + 1);
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = atom_obj;
  strcpy((char*) atom_of(o), name);
  return o;
}

obj* new_list() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = list_obj;
  list_of(o)->car = NULL;
  list_of(o)->cdr = NULL;
  return o;
}

obj* new_closure() {
  obj* o = malloc(sizeof(obj) + sizeof(closure_t));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = closure_obj;
  return o;
}

obj* copy_atom(const obj* o) {
  if (!is_atom(o)) return NULL;
  return new_atom(atom_of(o));
}

obj* copy_list(const obj *o) {
  obj* list_copy = new_list();
  memcpy(list_of(list_copy), list_of(o), sizeof(list_obj));
  return list_copy;
}

bool compare(const obj* a, const obj* b) {
  if (a == NULL || b == NULL) return a == b;
  if (a->objtype != b->objtype) return false;
  if (is_int(a)) return get_int(a) == get_int(b);
  if (is_float(a)) return get_float(a) == get_float(b);

  if (is_primitive(a))
    return *primitive_of(a) == *primitive_of(b);
  if (is_list(a))
    return memcmp(list_of(a), list_of(b), sizeof(list_t)) == 0;
  if (is_atom(a))
    return strcmp(atom_of(a), atom_of(b)) == 0;
  if (is_closure(a))
    return memcmp(closure_of(a), closure_of(b), sizeof(closure_t)) == 0;
  return false;
}

void dispose(obj* o) {
  free(o);
}

list_t* list_of(const obj *o) {
  return (list_t*) get_contents(o);
}

atom_t atom_of(const obj *o) {
  return (atom_t) get_contents(o);
}

closure_t* closure_of(const obj* o) {
  return (closure_t*) get_contents(o);
}

void* get_contents(const obj *o) {
  if (o == NULL) return NULL;
  return (void*) ((char*) o + sizeof(obj));
}

obj* new_int(int value) {
  obj* o = malloc(sizeof(obj) + sizeof(int));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = integer_obj;
  int* contents = (int*) get_contents(o);
  *contents = value;
  return o;
}

obj* new_float(float value) {
  obj* o = malloc(sizeof(obj) + sizeof(float));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = float_obj;
  float* contents = (float*) get_contents(o);
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
  return o->objtype == integer_obj;
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
  return strcmp(atom_of(o), "t") == 0 || strcmp(atom_of(o), "true") == 0;
}

float get_float(const obj* o) {
  if (is_int(o)) return (float) get_int(o);
  if (is_float(o)) return *(float*) ((char*) o + sizeof(obj));
  LOG_ERROR("Object is not a number");
  return 0;
}

int get_int(const obj* o) {
  if (is_float(o)) return (int) get_float(o);
  if (is_int(o)) return *(int*) ((char*) o + sizeof(obj));
  LOG_ERROR("Object is not a number");
  return 0;
}
