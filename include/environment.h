/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _LISP_ENV_H
#define _LISP_ENV_H

#include <primitives.h>

/**
 * Function: initEnv
 * -----------------
 * Initializes the default lisp environment
 * @return : Pointer to lisp environment object
 */
obj* init_env();

#endif