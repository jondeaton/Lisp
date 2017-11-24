/*
 * File: list.c
 * ------------
 * Implements the list data structure along with basic operations
 * such as copying and and disposal
 */

#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <closure.h>

// Static function declarations
static obj* copy_list_recursive(const obj *o);
static obj* copy_primitive(const obj* o);

obj* new_list_set(obj* car, obj* cdr) {
  obj* list = new_list();
  list_of(list)->car = car;
  list_of(list)->cdr = cdr;
  return list;
}

// Copy an object recursively
obj* copy_recursive(const obj *o) {
  if (o == NULL) return NULL;

  // Different kind of copying for each object type
  if (is_atom(o)) return copy_atom(o);
  if (is_primitive(o)) return copy_primitive(o);
  if (is_list(o)) return copy_list_recursive(o);
  if (is_int(o)) return new_int(get_int(o));
  if (is_float(o)) return new_float(get_float(o));
  if (is_closure(o)) copy_closure_recursive(o);
  return NULL;
}

void dispose_recursive(obj *o) {
  if (o == NULL) return;
  if (is_list(o)) { // Recursive disposal of lists and closures
    list_t *l = list_of(o);
    dispose_recursive(l->car);
    dispose_recursive(l->cdr);
  } else if (is_closure(o)) {
    closure_t* closure = closure_of(o);
    dispose_recursive(closure->parameters);
    dispose_recursive(closure->procedure);
    dispose_recursive(closure->captured);
  }
  dispose(o);
}

bool is_empty(const obj* o) {
  if (o == NULL) return false;
  if (!is_list(o)) return false;
  return list_of(o)->car == NULL && list_of(o)->cdr == NULL;
}

bool compare_recursive(const obj *x, const obj *y) {
  if (x->objtype != y->objtype) return false;
  if (is_atom(x)) return strcmp(atom_of(x), atom_of(y)) == 0;
  if (is_primitive(x)) return primitive_of(x) == primitive_of(y);

  // List: cars must match and cdrs must match
  if (is_list(x))
    return compare_recursive(list_of(x)->car, list_of(y)->car) && compare_recursive(list_of(x)->cdr, list_of(y)->cdr);
  else return x == y;
}

obj* ith(const obj* o, int i) {
  if (o == NULL || i < 0 || !is_list(o)) return NULL;
  if (i == 0) return list_of(o)->car;
  return ith(list_of(o)->cdr, i - 1);
}

int list_length(const obj* o) {
  if (o == NULL) return 0;
  if (!is_list(o)) return 0;
  return 1 + list_length(list_of(o)->cdr);
}

bool list_contains(const obj* list, const obj* query) {
  if (query == NULL || list == NULL) return false;
  bool found_here = compare_recursive(list_of(list)->car, query);
  return found_here || list_contains(list_of(list)->cdr, query);
}

/**
 * Function: copy_list
 * -------------------
 * Make a deep copy of a list object. All elements that the list points to will be copied as well
 * @param o: An object that is a list to copy
 * @return: A pointer to a new list object
 */
static obj* copy_list_recursive(const obj *o) {
  if (o == NULL) return NULL;

  obj* list_copy = new_list();
  list_of(list_copy)->car = copy_recursive(list_of(o)->car);
  list_of(list_copy)->cdr = copy_list_recursive(list_of(o)->cdr);
  return list_copy;
}

/**
 * Function: copy_primitive
 * ------------------------
 * Copy a list object that contains a primitive
 * @param o: A list object that is a primitive
 * @return: A deep copy of the list object
 */
static obj* copy_primitive(const obj* o) {
  if (o == NULL) return NULL;

  obj* new_primitive_obj = malloc(sizeof(obj) + sizeof(primitive_t));
  new_primitive_obj->objtype = primitive_obj; // New object should also be a primitive

  // Get pointer to the function pointers
  primitive_t* new_prim = primitive_of(new_primitive_obj);
  primitive_t* old_prim = primitive_of(o);

  // Copy the function pointer over
  memcpy(new_prim, old_prim, sizeof(primitive_t));
  return new_primitive_obj;
}
