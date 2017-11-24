/*
 * File: closures.h
 */

#ifndef _CLOSURE_H_INCLUDED
#define _CLOSURE_H_INCLUDED

#include "lisp-objects.h"

/**
 * Function: make_closure
 * ----------------------
 * Promote a lambda function to a closure by capturing variables from
 * the environment
 * @param lambda: A lambda expression to promote to a closure
 * @return: A closure object
 */
obj* make_closure(const obj *lambda, obj *env);

/**
 * Function: new_closure_set
 * -------------------------
 * Create a new closure with initialization of fields. The passed objects will not be copied.
 * @param params: Parameters to the closure
 * @param procedure: Procedure of the closure
 * @param captured: Captured argument list of the closure
 * @return: A new closure object with the specified parameters, procedure, and captured vars list.
 */
obj *new_closure_set(obj *params, obj *procedure, obj *captured);

/**
 * Function: copy_closure_recursive
 * --------------------------------
 * Make a deep copy of a closure
 * @param closure: The closure to make a copy of
 * @return: A completely newly allocated closure identical to the passed one
 */
obj* copy_closure_recursive(const obj* closure);

/**
 * Function: get_parameters
 * ------------------------
 * Get the parameter list of a lambda expression
 * @param lambda: The lambda expression to get the parameters of
 * @return: The list of parameters in the lambda function definition
 */
obj* get_lambda_parameters(const obj *lambda);

/**
 * Function: get_body
 * ------------------
 * Get the body of a lambda expression
 * @param lambda: The lambda expression to get the body of
 * @return: The body of the lambda expression
 */
obj* get_lambda_body(const obj *lambda);

#endif // _CLOSURE_H_INCLUDED
