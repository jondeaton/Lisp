/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure
 */

#ifndef _LIST_H
#define _LIST_H

typedef char* atom_t;

enum type {atom_obj, list_obj, func_obj, primitive_obj};

typedef struct {
  enum type objtype;
} obj;

typedef struct {
  obj* car;
  obj* cdr;
} list_t;

typedef obj*(*primitive_t)(obj*);

/**
 * Function: getList
 * -----------------
 * Gets a pointer to the list
 * @param o : Pointer to a lisp data structure
 * @return : A pointer to that list data structure's list
 */
list_t* getList(obj* o);

/**
 * Function: getAtom
 * -----------------
 * Gets a pointer to the atom
 * @param o : Pointer to a lisp data structure
 * @return : A pointer to the atom in the object
 */
atom_t getAtom(obj* o);

/**
 * Function: getPrimitive
 * ----------------------
 * Gets a funciton pointer to the primitive function for a primitive object list
 * @param o : Pointer to a lisp data structure
 * @return : Function pointer to primitive function
 */
primitive_t getPrimitive(obj* o);

/**
 * Function: dispose
 * -----------------
 * Frees the dynamically allocated memory used to store
 * the lisp object.
 * @param o : Pointer to the lisp objct to dispose of
 */
void dispose(obj* o);

#endif