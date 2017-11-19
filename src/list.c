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

// Static function declarations
static obj* copy_list(const obj* o);
static obj* copy_atom(const obj* o);
static obj* copy_primitive(const obj* o);

// Copy an object recursively
obj* copy_recursive(const obj *o) {
  if (o == NULL) return NULL;

  // Different kind of copying for each object type
  if (o->objtype == atom_obj) return copy_atom(o);
  if (o->objtype == primitive_obj) return copy_primitive(o);
  if (o->objtype == list_obj) return copy_list(o);
  return NULL;
}

void dispose_recursive(obj *o) {
  if (o == NULL) return;
  if (o->objtype == list_obj) { // Recursive disposal of lists
    list_t *l = list_of(o);
    dispose_recursive(l->car);
    dispose_recursive(l->cdr);
  }
  free(o);
}

bool is_empty(const obj* o) {
  if (o == NULL) return false;
  if (o->objtype != list_obj) return false;
  return list_of(o)->car == NULL && list_of(o)->cdr == NULL;
}

bool deep_compare(obj* x, obj* y) {
  if (x->objtype != y->objtype) return false;
  if (x->objtype == atom_obj) return strcmp(atom_of(x), atom_of(y)) == 0;
  if (x->objtype == primitive_obj) return primitive_of(x) == primitive_of(y);

  // List: cars must match and cdrs must match
  if (x->objtype == list_obj)
    return deep_compare(list_of(x)->car, list_of(y)->car) && deep_compare(list_of(x)->cdr, list_of(y)->cdr);
  else return x == y;
}

obj* ith(const obj* o, int i) {
  if (o == NULL || i < 0 || o->objtype != list_obj) return NULL;
  if (i == 0) return list_of(o)->car;
  return ith(list_of(o)->cdr, i - 1);
}

/**
 * Function: copy_atom
 * -------------------
 * Copy an object that is an atom by dynamically allocating space for an identical object, and then
 * copying the contents of the atom over into the new object.
 * @param o: The object (of type atom) to copy
 * @return: A pointer to a copy of the object in dynamically allocated space
 */
static obj* copy_atom(const obj* o) {
  if (o == NULL) return NULL;

  atom_t atom_source = atom_of(o); // The atom contents to be copied
  size_t atom_size = strlen(atom_source); // Get the size of the atom to be copied

  obj* obj_copy = malloc(sizeof(obj) + atom_size + 1);   // Allocate space for copy
  obj_copy->objtype = atom_obj; // The copy is also an atom
  strcpy(get_contents(obj_copy), atom_source); // Copy the atom contents itself
  return obj_copy;
}

/**
 * Function: copy_list
 * -------------------
 * Make a deep copy of a list object. All elements that the list points to will be copied as well
 * @param o: An object that is a list to copy
 * @return: A pointer to a new list object
 */
static obj* copy_list(const obj* o) {
  if (o == NULL) return NULL;

  obj* list_copy = new_list();
  list_of(list_copy)->car = copy_recursive(list_of(o)->car);
  list_of(list_copy)->cdr = copy_list(list_of(o)->cdr);

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
