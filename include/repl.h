/*
 * File: repl.h
 * ------------
 * Presents the interface to the Read-Eval-Print Loop
 */

#ifndef _REPL_H_INCLUDED
#define _REPL_H_INCLUDED

#include "primitives.h"
#include "parser.h"
#include <stdio.h>

/**
 * Function: repl_init
 * -------------------
 * Initializes the read eval print loop by creating
 * the global environment and the vector of allocated
 * objects.
 */
void repl_init();

/**
 * Function: repl_run_program
 * --------------------------
 * Reads in expressions from a file, and runs them
 * in the environment initialized in repl_init
 * @param program_file: The file containing the program to run
 */
void repl_run_program(const char* program_file);

/**
 * Function: repl_run
 * ------------------
 * Run the interactive REPL prompt
 */
void repl_run();

/**
 * Function: repl_eval
 * -------------------
 * Evaluate a single expression in the global environment
 * @param expr: The expression to evaluate
 * @return: The an expression representing the result of evaluating
 * the passed expression, in dynamically allocated memory that must
 * be freed
 */
expression repl_eval(const_expression expr);

/**
 * Function: repl_dispose
 * ----------------------
 * Dispose of the global environment and vector of allocated objects.
 */
void repl_dispose();

#endif //_REPL_H_INCLUDED