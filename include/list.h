/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure
 */

#ifndef _LIST_H
#define _LIST_H

#include <stdbool.h>

typedef char* atom_t;

enum type {atom_obj, list_obj, closure_obj, primitive_obj};

typedef struct {
  enum type objtype;
} obj;

typedef struct {
  obj* car;
  obj* cdr;
} list_t;

typedef obj*(*primitive_t)(obj*, obj*);

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
 * @param o : Pointer to the lisp objct to dispose of
 */
void dispose(obj* o);

/**
 * Function: getList
 * -----------------
 * Gets a pointer to the list
 * @param o : Pointer to a lisp data structure
 * @return : A pointer to that list data structure's list
 */
list_t* get_list(const obj *o);

/**
 * Function: getAtom
 * -----------------
 * Gets a pointer to the atom
 * @param o : Pointer to a lisp data structure
 * @return : A pointer to the atom in the object
 */
atom_t get_atom(const obj *o);

/**
 * Function: getPrimitive
 * ----------------------
 * Gets a pointer to a function pointer to the primitive function for a primitive object list
 * @param o : Pointer to a lisp data structure
 * @return : Function pointer to primitive function
 */
primitive_t* get_primitive(const obj *o);


#endif