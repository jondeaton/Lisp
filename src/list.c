/*
 * File: list.c
 * ------------
 * Implements the list data structure along with basic operations
 * such as copying and and disposal
 */

#include "list.h"
#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* copy_list(const obj* o);
static obj* copy_atom(const obj* o);
static obj* copy_primitive(const obj* o);
static void* get_contents(const obj *o);

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
    list_t *l = get_list(o);
    dispose(l->car);
    dispose(l->cdr);
  }
  free(o);
}

list_t* get_list(const obj *o) {
  return (list_t*) get_contents(o);
}

atom_t get_atom(const obj *o) {
  return (atom_t) get_contents(o);
}

primitive_t* get_primitive(const obj *o) {
  return (primitive_t*) get_contents(o);
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

  atom_t atom_source = get_atom(o); // The atom contents to be copied

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

  obj* new_list = malloc(sizeof(obj) + sizeof(list_obj));
  new_list->objtype = list_obj; // New object is also a list

  // Recursive copying of car and cdr pointers
  get_list(new_list)->car = copy(get_list(o)->car);
  get_list(new_list)->cdr = copy(get_list(o)->cdr);

  return new_list;
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
  primitive_t* new_prim = get_primitive(new_primitive_obj);
  primitive_t* old_prim = get_primitive(o);

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