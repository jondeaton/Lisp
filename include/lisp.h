/**
 * File: lisp.h
 * ------------
 * Presents the interface to the lisp primitives
 */

#ifndef _lisp_
#define _lisp_

#include "stdbool.h"
#include "stdlib.h"

typedef char* expression_t;
typedef char* atom_t;

enum type {atom_obj, list_obj, primitive_obj};

typedef struct {
  enum type objtype;
	void* p; // Pointer to contents
} obj;

const obj t_atom = {
  .objtype = atom_obj,
  .p = "t"
};

const obj empty_atom = {
  .objtype = atom_obj,
  .p = "()"
};

const obj* t = &t_atom;
const obj* empty = &empty_atom;

typedef struct {
	obj* car;
  obj* cdr;
} list_t;

typedef obj*(*primitive_t)(obj*);

/**
 * Primitive: quote
 * ----------------
 * Returns the unevaluated version of the object
 * @param o : The object to quote
 * @return : Pointer to the object without evaluating it
 */
obj* quote(obj* o);

/**
 * Primitive: atom
 * ---------------
 * Checks if an object is an atom
 * @param o : An object to check if it is an atom
 * @return : t if it is an atom, else the empty list
 */
const obj* atom(obj* o);

/**
 * Primitive: eq
 * -------------
 * Test for equality of two objects
 * @param x : The first object
 * @param y : The second object
 * @return t if both are equal, empty list (false) otherwise
 */
const obj* eq(obj* o);

/**
 * Primitive: car
 * -------------
 * Expects the value of l to be a list and returns it's first element
 * @param o : A list object (will be asserted)
 * @return : The first element of the list
 */
obj* car(obj* o);

/**
 * Primitive: cdr
 * -------------
 * Expects the value of l to be a list and returns everything after the first element
 * @param o : A list object (will be asserted)
 * @return : Everything after the first element of the list
 */
obj* cdr(obj* o);

/**
 * Primitive: cons
 * -------------
 * Expects the value of y to be a list and returns a list containing the value
 * of x followed by the elements of the value of y 
 * @param x : Some object
 * @param y : 
 * @return
 */
obj* cons(obj* x, obj* y);

/**
 * Primitive: cons
 * ---------------
 * (cond (p1 e1) ... (pn en))
 * The p expressions are evaluated in order until one returns t
 * When one is found  the value of the corresp onding e expression
 * is returned as the expression
 * @param l :
 * @return
 */
obj* cond(obj* l);

/**
 * Primitive: label
 * ----------------
 * yeah...
 * @param o
 * @return
 */
obj* label(obj* o);

/**
 * Primitive: lambda
 * -----------------
 * The most important primitive...
 * @param args
 * @return
 */
obj* lambda(obj* o);

#endif