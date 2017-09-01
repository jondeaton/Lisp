/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _LISP_ENV_H
#define _LISP_ENV_H

#include <lisp.h>

/**
 * Function: initEnv
 * -----------------
 * Initializes the default lisp environment
 * @return : Pointer to lisp environment object
 */
obj* initEnv();

/**
 * Function: disposeEnv
 * --------------------
 * Disposes of the lisp environment object
 */
void disposeEnv();

#endif