/*
 * File: repl.h
 * ------------
 * Presents the interface to the Read-Eval-Print Loop
 */

#ifndef _REPL_H
#define _REPL_H

#include <stdbool.h>
#include <stdlib.h>
#include "lisp.h"

/**
 * Function: repl
 * --------------
 * The read-eval-print loop which
 * simply calls read, eval, and print
 * in that order over and over again.
 */
void repl();

/**
 * Function: read
 * --------------
 * Takes an expression from standard input, turns it into
 * an object and returns the object
 * @return : The object representation of the expression read from stdin
 */
obj* read();

/**
 * Function: print
 * ---------------
 * Converts an object to an expression and then prints it
 * to standard output
 * @param o : An object to print
 * @return : Nothing
 */
void print(obj* o);

#endif