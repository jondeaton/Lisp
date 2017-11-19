/*
 * File: lisp-objects.c
 * --------------------
 * Presents the implementation of lisp object creation
 */

#include "lisp-objects.h"
#include <stack-trace.h>
#include <stdlib.h>
#include <string.h>

obj* new_list() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  if (o == NULL) {
    log_error(__func__, "Allocation failure.");
    return NULL;
  }
  o->objtype = list_obj;
  list_of(o)->car = NULL;
  list_of(o)->cdr = NULL;
  return o;
}

obj* new_atom(atom_t name) {
  if (name == NULL) return NULL;
  size_t name_size = strlen(name);
  obj* o = malloc(sizeof(obj) + name_size + 1);
  if (o == NULL) {
    log_error(__func__, "Memory allocation failure.");
    return NULL;
  }
  o->objtype = atom_obj;
  strcpy((char*) atom_of(o), name);
  return o;
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  if (o == NULL) {
    log_error(__func__, "Allocation failure.");
    return NULL;
  }
  o->objtype = primitive_obj;
  memcpy(primitive_of(o), &primitive, sizeof(primitive));
  return o;
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
  if (o == NULL) {
    log_error(__func__, "Memory allocation failure");
    return NULL;
  }
  o->objtype = integer_obj;
  int* contents = (int*) get_contents(o);
  *contents = value;
  return o;
}

obj* new_float(float value) {
  obj* o = malloc(sizeof(obj) + sizeof(float));
  if (o == NULL) {
    log_error(__func__, "Memory allocation failure");
    return NULL;
  }
  o->objtype = float_obj;
  float* contents = (float*) get_contents(o);
  *contents = value;
  return o;
}

float get_float(const obj* o) {
  if (o->objtype == integer_obj) return (float) get_int(o);
  if (o->objtype == float_obj) return *(float*) ((char*) o + sizeof(obj));
  log_error(__func__, "Object is not a number");
  return 0;
}

int get_int(const obj* o) {
  if (o->objtype == float_obj) return (int) get_float(o);
  if (o->objtype == integer_obj) return *(int*) ((char*) o + sizeof(obj));
  log_error(__func__, "Object is not a number");
  return 0;
}