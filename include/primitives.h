/*
 * File: lisp.h
 * ------------
 * Presents the interface to the lisp primitives
 */

#ifndef _LISP_H_INCLUDED
#define _LISP_H_INCLUDED

#include <list.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Primitive: quote
 * ----------------
 * Returns the unevaluated version of the object
 * @param o: The object to quote
 * @param env: The environment to evaluate this primitive in
 * @return: Pointer to the object without evaluating it
 */
obj* quote(obj* o, obj* env);

/**
 * Primitive: atom
 * ---------------
 * Checks if an object is an atom
 * @param o: An object to check if it is an atom
 * @param env: The environment to evaluate this primitive in
 * @return: t if it is an atom, else the empty list
 */
obj* atom(obj* o, obj* env);

/**
 * Primitive: eq
 * -------------
 * Test for equality of two objects
 * @param o: pointer to a list containing the first object and then the second object
 * @param env : The environment to evaluate this primitive in
 * @return t if both are equal, empty list (false) otherwise
 */
obj* eq(obj* o, obj* env);

/**
 * Primitive: car
 * -------------
 * Expects the value of l to be a list and returns it's first element
 * @param o : A list object (will be asserted)
 * @param env : The environment to evaluate this primitive in
 * @return : The first element of the list
 */
obj* car(obj* o, obj* env);

/**
 * Primitive: cdr
 * -------------
 * Expects the value of l to be a list and returns everything after the first element
 * @param o : A list object (will be asserted)
 * @param env : The environment to evaluate this primitive in
 * @return : Everything after the first element of the list
 */
obj* cdr(obj* o, obj* env);

/**
 * Primitive: cons
 * ---------------
 * Expects the value of y to be a list and returns a list containing the value
 * of x followed by the elements of the value of y 
 * @param o : the argument to the cons call
 * @param env : The environment to evaluate this primitive in
 * @return
 */
obj* cons(obj* o, obj* env);

/**
 * Primitive: cons
 * ---------------
 * (cond (p1 e1) ... (pn en))
 * The p expressions are evaluated in order until one returns t
 * When one is found  the value of the corresp onding e expression
 * is returned as the expression
 * @param o : The argument to the cond call
 * @param env : The environment to evaluate this primitive in
 * @return
 */
obj* cond(obj* o, obj* env);

#endif // _LISP_H_INCLUDED