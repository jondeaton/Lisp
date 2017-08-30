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
 * @return : The object representation of the expression read from stdin
 */
obj* read();

/**
 * Function: eval
 * --------------
 * Takes an object (either a list or an atom) and returns
 * the result of the evaluation of this
 * @param o : An object to be evaluated
 * @return : An reduced object
 */
obj* eval(obj* o);

/**
 * Function: print
 * ---------------
 * Converts an object to an expression and then prints it
 * to standard output
 * @param o : An object to print
 * @return : Nothing
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