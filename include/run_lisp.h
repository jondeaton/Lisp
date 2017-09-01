/*
 * File: run_lisp.h
 * ----------------
 * Exports the interface for running lisp program files
 */

#ifndef _LISP_RUN_PROG_H
#define _LISP_RUN_PROG_H

/**
 * Function: run
 * -------------
 * Runs a lisp program stored in a file
 * @param lispProgramPath : The path to the lisp program
 */
void run(const char const* lispProgramPath);

#endif
