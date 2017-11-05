/*
 * File: parser.h
 * --------------
 * Presents the lisp parser interface
 */

#ifndef _LISP_PARSER_H
#define _LISP_PARSER_H

#include <primitives.h>

typedef char* expression_t;

/**
 * Function: parseExpression
 * -------------------------
 * Parses a lisp expression that represents either a lisp atom or list
 * @param e : A balanced, valid lisp expression
 * @param num_parsed_p : A pointer to a place where the number of parsed characters may be written. Must be valid
 * @return : Pointer to a lisp data structure object representing that the lisp expression represents
 */
obj* parse_expression(expression_t e, size_t *num_parsed_p);

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
bool is_balanced(expression_t e);

/**
 * Function: isValid
 * -----------------
 * Checks if the expression is valid
 * @param e : The expression to check
 * @return : True if the expression is valid, false otherwise
 */
bool isValid(expression_t e);

#endif