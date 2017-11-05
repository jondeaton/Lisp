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

// Static function declarations
static obj* copy_list(const obj* o);
static obj* copy_atom(const obj* o);
static obj* copy_primitive(const obj* o);
static void* get_contents(const obj *o);

obj* new_list() {
  obj* o = malloc(sizeof(obj) + sizeof(list_t));
  if (o == NULL) return NULL;
  o->objtype = list_obj;
  list_of(o)->car = NULL;
  list_of(o)->cdr = NULL;
  return o;
}

// Copy an object recursively
obj* copy(const obj* o) {
  if (o == NULL) return NULL;

  // Different kind of copying for each object type
  if (o->objtype == atom_obj) return copy_atom(o);
  if (o->objtype == primitive_obj) return copy_primitive(o);
  if (o->objtype == list_obj) return copy_list(o);
  return NULL;
}

// Recursive disposal of an object
void dispose(obj* o) {
  if (o == NULL) return;
  if (o->objtype == list_obj) {
    list_t *l = list_of(o);
    dispose(l->car);
    dispose(l->cdr);
  }
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

  // Get the size of the atom to be copied
  size_t atom_size = strlen(atom_source);

  // Allocate space for copy
  obj* obj_copy = malloc(sizeof(obj) + atom_size + 1);

  // The copy is also an atom
  obj_copy->objtype = atom_obj;

  // Copy the atom contents itself
  strcpy(get_contents(obj_copy), atom_source);
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
  list_of(list_copy)->car = copy(list_of(o)->car);
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

/**
 * Function: get_contents
 * ----------------------
 * Utility function for getting the contents of the object that exists just to the right of
 * the object type enum.
 */
static void* get_contents(const obj *o) {
  return (void*) ((char*) o + sizeof(obj));
}