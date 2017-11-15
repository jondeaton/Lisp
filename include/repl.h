/*
 * File: repl.h
 * ------------
 * Presents the interface to the Read-Eval-Print Loop
 */

#ifndef _REPL_H_INCLUDED
#define _REPL_H_INCLUDED

#include <primitives.h>
#include <stdio.h>

// todo: document what these do
void repl_init();
void repl_run_program(const char* program_file);
void repl_run();
void repl_eval();
void repl_dispose();

// todo: delete these stupid functions

/**
 * Function: repl
 * --------------
 * The read-eval-print loop which
 * simply calls read, eval, and print
 * in that order over and over again.
 * @return : Exit status
 */
int repl();

/**
 * Function: read_expression
 * -------------------------
 * Takes an expression from standard input, turns it into
 * an object and returns the object
 *
 * @return: The object representation of the expression read from standard input
 */
obj* read_expression(FILE *fd, const char *prompt, const char *reprompt);

/**
 * Function: print
 * ---------------
 * Converts an object to an expression and then prints it
 * to standard output
 * @param o: An object to print
 */
void print(FILE* fd, obj* o);

#endif //_REPL_H_INCLUDED