/*
 * File: run_lisp.h
 * ----------------
 * Exports the interface for running lisp program files
 */

#ifndef _RUN_LISP_H
#define _RUN_LISP_H

#include <list.h>

/**
 * Function: run
 * -------------
 * Runs a lisp program stored in a file
 * @param lispProgramPath : The path to the lisp program
 * @param env : Environment to run the program in
 * @return : Exit status
 */
int run(const char* lispProgramPath, obj* env);

#endif
