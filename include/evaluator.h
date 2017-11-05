/*
 * File: evaluator.h
 * -----------------
 * Presents the interface to the
 */

#ifndef _LISP_EVALUATOR_H
#define _LISP_EVALUATOR_H

#include <list.h>

/**
 * Function: eval
 * --------------
 * Evaluates a lisp object in an environment
 * @param o : An object to be evaluated
 * @param env : Environment to evaluate the expression in
 * @return : The result of the evaluation of the object
 */
obj* eval(const obj* o, obj* env);

/**
 * Function: apply
 * ---------------
 * Applies a lisp function to an argument list
 * @param o : A lisp function object (will be asserted)
 * @param args : List of arguments
 * @param env : Environment to evaluate the expression in
 * @return : The result of the application of the function to the arguments
 */
obj* apply(const obj* closure, const obj* args, obj* env);

#endif