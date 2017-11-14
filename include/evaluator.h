/*
 * File: evaluator.h
 * -----------------
 * Presents the interface to the
 */

#ifndef _LISP_EVALUATOR_H
#define _LISP_EVALUATOR_H

#include "list.h"
#include "cvector.h"
#include <stdlib.h>

// Vector of allocated objects needing to be free'd
extern CVector* allocated;

/**
 * Function: eval
 * --------------
 * Evaluates a lisp object in an environment, potentially allocating
 * objects in the global allocated vector.
 * @param o: An object to be evaluated
 * @param env: Environment to evaluate the expression in
 * @return: The result of the evaluation of the object
 */
obj* eval(const obj* o, obj* env);

/**
 * Function: apply
 * ---------------
 * Applies a lisp function to an argument list
 * @param operator: A lisp function object (will be asserted)
 * @param args: List of arguments
 * @param env: Environment to evaluate the expression in
 * @return: The result of the application of the function to the arguments
 */
obj* apply(const obj* operator, const obj* args, obj* env);

/**
 * Function: init_allocated
 * ------------------------
 * Initializes the CVector of pointers to allocated objects
 */
void init_allocated() { allocated = cvec_create(sizeof(obj*), 0, &free); }

/**
 * Function: clear_allocated
 * -------------------------
 * Frees all of the allocated lisp objects
 */
void clear_allocated() { cvec_clear(allocated); }

#endif // _LISP_EVALUATOR_H