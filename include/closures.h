/*
 * File: closures.h
 */

#ifndef _CLOSURE_H_INCLUDED
#define _CLOSURE_H_INCLUDED

#include <lisp-objects.h>

/**
 * Function: make_closure
 * ----------------------
 * Promote a lambda function to a closure by capturing it's environment
 * @param lambda: A lambda expression to promote to a closure
 * @return: A closure object
 */
obj* make_closure(const obj* lambda, obj** envp);

#endif // _CLOSURE_H_INCLUDED
