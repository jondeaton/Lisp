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
