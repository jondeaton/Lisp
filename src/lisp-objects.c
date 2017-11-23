/*
 * File: lisp-objects.c
 * --------------------
 * Presents the implementation of lisp object creation
 */

#include <lisp-objects.h>
#include <stack-trace.h>
#include <stdlib.h>
#include <string.h>

obj* new_list() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = list_obj;
  list_of(o)->car = NULL;
  list_of(o)->cdr = NULL;
  return o;
}

obj* new_atom(atom_t name) {
  if (name == NULL) return NULL;
  size_t name_size = strlen(name);
  obj* o = malloc(sizeof(obj) + name_size + 1);
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = atom_obj;
  strcpy((char*) atom_of(o), name);
  return o;
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  if (o == NULL) return LOG_MALLOC_FAIL();
  o->objtype = primitive_obj;
  memcpy(primitive_of(o), &primitive, sizeof(primitive));
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
  if (a == NULL) return b == NULL;
  if (a->objtype != b->objtype) return false;
  if (a->objtype == integer_obj) return get_int(a) == get_int(b);
  if (a->objtype == float_obj) return get_float(a) == get_float(b);

  if (a->objtype == primitive_obj)
    return *primitive_of(a) == *primitive_of(b);
  if (a->objtype == list_obj)
    return memcmp(list_of(a), list_of(b), sizeof(list_t)) == 0;
  if (a->objtype == atom_obj)
    return strcmp(atom_of(a), atom_of(b)) == 0;
//  if (a->objtype == closure_obj)
//    return memcmp(closure_of(a), closure_of(b), sizeof(closure_t));
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

primitive_t* primitive_of(const obj *o) {
  return (primitive_t*) get_contents(o);
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
  return o->objtype == atom_obj;
}

bool is_primitive(const obj* o) {
  return o->objtype == primitive_obj;
}

bool is_list(const obj* o) {
  return o->objtype == list_obj;
}

bool is_closure(const obj* o) {
  return o->objtype == closure_obj;
}

bool is_int(const obj* o) {
  return o->objtype == integer_obj;
}

bool is_float(const obj* o) {
  return o->objtype == float_obj;
}

bool is_number(const obj* o) {
  return is_float(o) || is_int(o);
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
