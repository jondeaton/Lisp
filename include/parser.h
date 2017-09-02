/*
 * File: parser.h
 * --------------
 * Presents the lisp parser interface
 */

#ifndef _LISP_PARSER_H
#define _LISP_PARSER_H

#include <lisp.h>

typedef char* expression_t;

/**
 * Function: parse
 * ---------------
 * Converts a lisp expression into a lisp data structure
 * @param input : The lisp expression to objectify
 * @return : Pointer to the lisp data structure
 */
obj* parse(expression_t e, size_t* numParsedP);

/**
 * Function: unparse
 * -----------------
 * Stringifies a lisp data structure. Will return a pointer to dynamically
 * allocated memory that must be freed.
 * @param o : A lisp object
 * @return : The string representation of the lisp data structure
 */
expression_t unparse(obj* o);

/**
 * Function: unbalanced
 * ------------------------------
 * Checks if all of the open parenthesis in the expression
 * are balanced by a closing parentheses.
 * @param e : A lisp expression
 * @return : True if there are at least as many
 */
bool isBalanced(expression_t e);

/**
 * Function: isValid
 * -----------------
 * Checks if the expression is valid
 * @param e : The expression to check
 * @return : True if the expression is valid, false otherwise
 */
bool isValid(expression_t e);

#endif