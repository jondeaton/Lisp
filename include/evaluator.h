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
 * Initializes the CVector of pointers to allocated objects. Call this
 * before any calls to eval are made. Calling eval without first calling this
 * method results in undefined behavior.
 */
void init_allocated();

/**
 *  Function: add_allocated
 * -----------------------
 * Add a pointer to a lisp object in dynamically allocated memory that
 * needs to be freed.
 * @param o: A pointer to a lisp object that needs to be free'd
 */
void add_allocated(const obj* o);

/**
 * Function: clear_allocated
 * -------------------------
 * Frees all of the allocated lisp objects. Suggested usage is to
 * call this function after each use of eval, once the object returned
 * from eval has been copied/processed.
 */
void clear_allocated();

/**
 * Function: dispose_allocated
 * ---------------------------
 * Disposes of the CVector of allocated objects. Call this method after
 * all calls to eval are completed to free the memory used to store the
 * CVector.
 */
void dispose_allocated();

#endif // _LISP_EVALUATOR_H