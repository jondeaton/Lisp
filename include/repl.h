/*
 * File: run-lisp.h
 * ----------------
 * Exports the interface for running the lisp interpreter
 */

#ifndef _RUN_LISP_H_INCLUDED
#define _RUN_LISP_H_INCLUDED

#include <stdbool.h>

/**
 * Function: run
 * -------------
 * Run the lisp interpreter optionally bootstrapping from a file, running
 * a lisp program, or running the interactive prompt
 * @param lispProgramPath: The path to the lisp program
 * @param env: Environment to run the program in
 * @return: Exit status
 */
int
run_lisp(const char *bootstrap_path, const char *program_file, bool run_repl, const char *history_file, bool verbose);

#endif //_RUN_LISP_H_INCLUDED
