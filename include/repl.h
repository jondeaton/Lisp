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
 * @param e
 * @return
 */
obj* read(expression e);
obj* eval(obj* o);
expression print(obj* o);
void repl();


static bool isWhiteSpace(char character);

#endif