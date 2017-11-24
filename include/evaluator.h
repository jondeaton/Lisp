/*
 * File: evaluator.h
 * -----------------
 * Presents the interface to the
 */

#ifndef _EVALUATOR_H_INCLUDED
#define _EVALUATOR_H_INCLUDED

#include "lisp-objects.h"

/**
 * Function: eval
 * --------------
 * Evaluates a lisp object in an environment, potentially allocating
 * objects in the global allocated vector.
 * @param o: An object to be evaluated
 * @param envp: Environment to evaluate the expression in
 * @return: The result of the evaluation of the object
 */
obj* eval(const obj* o, obj** envp);

/**
 * Function: apply
 * ---------------
 * Applies a lisp function to an argument list
 * @param operator: A lisp function object (will be asserted)
 * @param args: List of arguments
 * @param envp: Environment to evaluate the expression in
 * @return: The result of the application of the function to the arguments
 */
obj* apply(const obj* operator, const obj* args, obj** envp);

#endif // _EVALUATOR_H_INCLUDED
