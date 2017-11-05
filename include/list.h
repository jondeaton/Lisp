/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure along with basic operations
 * such as copying and and disposal
 */

#ifndef _LIST_H_INCLUDED
#define _LIST_H_INCLUDED

#include <stdbool.h>

// The different types of lists
enum type {atom_obj, list_obj, primitive_obj};

typedef char* atom_t;

typedef struct {
  enum type objtype;
} obj;

typedef struct {
  obj* car;
  obj* cdr;
} list_t;

typedef obj*(*primitive_t)(const obj*, obj*);

/**
 * Function: new_list
 * ------------------
 * Returns a list object in dynamically allocated memory
 * @return: A pointer to a new list object in dynamically allocated memory
 */
obj* new_list();

/**
 * Function: copy
 * --------------
 * Copies an object, returning a new one, leaving the old one untouched
 * @param o: An object to copy
 * @return: A copy of the object
 */
obj* copy(const obj* o);

/**
 * Function: dispose
 * -----------------
 * Frees the dynamically allocated memory used to store
 * the lisp object.
 * @param o: Pointer to the lisp objct to dispose of
 */
void dispose(obj* o);

/**
 * Function: get_list
 * ------------------
 * Gets a pointer to the list
 * @param o: Pointer to a lisp data structure
 * @return: A pointer to that list data structure's list
 */
list_t* list_of(const obj *o);

/**
 * Function: get_atom
 * ------------------
 * Gets a pointer to the atom
 * @param o: Pointer to a lisp data structure
 * @return: A pointer to the atom in the object
 */
atom_t atom_of(const obj *o);

/**
 * Function: get_primitive
 * -----------------------
 * Gets a pointer to a function pointer to the primitive function for a primitive object list
 * @param o: Pointer to a lisp data structure
 * @return: Function pointer to primitive function
 */
primitive_t* primitive_of(const obj *o);

/**
 * Function: is_empty
 * ------------------
 * @param o: A lisp object to determine if it is the empty list
 * @return: True if the object is the empty list, false otherwise
 */
bool is_empty(const obj* o);

/**
 * Function: deep_compare
 * ----------------------
 * Deep comparison of two lisp objects
 * @param x: The first object to compare
 * @param y: The second object to compare
 * @return: True if the two objects are identical, false otherwise
 */
bool deep_compare(obj* x, obj* y);

#endif // _LIST_H_INCLUDED