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
 * Initializes a CVector of pointers to allocated objects. Note that this list
 * should not contain the environment, or parsed lisp objects. This list should
 * only contain lisp objects that needed to be created during calls to eval.
 * Call this before any calls to eval are made. Calling eval without first calling this
 * method results in undefined behavior.
 */
void init_allocated();

/**
 *  Function: add_allocated
 * -----------------------
 * Adds a lisp object that was created during evaluation and needs to be
 * freed. This method should be called by any function internal to
 * eval that allocated lisp objects in dynamically allocated memory.
 * @param o: A pointer to a lisp object that needs to be free'd
 */
void add_allocated(const obj* o);

/**
 * Function: clear_allocated
 * -------------------------
 * Frees all of the allocated lisp objects in the allocated list. Suggested usage is to
 * call this function after each call to eval, after the object returned from
 * eval has been completely processed (e.g. copied into environment, serialized, etc...).
 */
void clear_allocated();

/**
 * Function: dispose_allocated
 * ---------------------------
 * Disposes of the CVector of allocated objects. Call this method after
 * all calls to eval are completed to free the memory used to store the
 * CVector of allocated objects.
 */
void dispose_allocated();

#endif // _LISP_EVALUATOR_H