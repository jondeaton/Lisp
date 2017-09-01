/*
 * File: parser.h
 * --------------
 * Presents the lisp parser interface
 */

#ifndef _LISP_PARSER_H
#define _LISP_PARSER_H

#include <lisp.h>

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

#endif