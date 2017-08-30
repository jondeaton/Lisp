/*
 * File: repl.h
 * ------------
 * Presents the interface to the Read-Eval-Print Loop
 */

#ifndef _repl_
#define _repl_

#include "lisp.h"
#include "stdbool.h"
#include "stdlib.h"

typedef char* expression;

/**
 * Function: read
 * --------------
 * Takes an expression from standard input, turns it into
 * an object and returns the object
 * @return
 */
obj* read();


/**
 * Function: eval
 * --------------
 * Takes an object (either a list or an atom) and returns
 *
 * @param o
 * @return
 */
obj* eval(obj* o);

/**
 *
 * @param o
 * @return
 */
void print(obj* o);

/**
 * Function: repl
 * --------------
 * The read-eval-print loop which
 * simply calls read, eval, and print
 * in that order over and over again.
 */
void repl();

#endif