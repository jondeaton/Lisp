/*
 * File: parser.h
 * --------------
 * Presents the lisp parser interface
 */

#ifndef _PARSER_H_INCLUDED
#define _PARSER_H_INCLUDED

#include "primitives.h"
#include <stdlib.h>

typedef char* expression;
typedef const char* const_expression;

/**
 * Function: parse_expression
 * --------------------------
 * Parses a lisp expression that represents either a lisp atom or list
 * @param e: A balanced, valid lisp expression
 * @param num_parsed_p: A pointer to a place where the number of parsed characters may be written. Must be valid
 * @return: Pointer to a lisp data structure object representing that the lisp expression represents
 */
obj* parse_expression(const_expression e, size_t *num_parsed_p);

/**
 * Function: unparse
 * -----------------
 * Stringifies a lisp data structure. Will return a pointer to dynamically
 * allocated memory that must be freed.
 * @param o: A lisp object
 * @return: The string representation of the lisp data structure
 */
expression unparse(const obj* o);

/**
 * Function: is_balanced
 * ---------------------
 * Checks if all of the open parenthesis in the expression
 * are balanced by a closing parentheses.
 * @param e: A lisp expression
 * @return: True if there are at least as many
 */
bool is_balanced(const_expression e);

/**
 * Function: is_valid
 * ------------------
 * Checks if the expression is valid
 * @param e: The expression to check
 * @return: True if the expression is valid, false otherwise
 */
bool is_valid(const_expression e);

#endif // _PARSER_H_INCLUDED
