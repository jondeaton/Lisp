/*
 * File: run_lisp.h
 * ----------------
 * Exports the interface for running lisp program files
 */

#ifndef _RUN_LISP_H
#define _RUN_LISP_H

/**
 * Function: run
 * -------------
 * Runs a lisp program stored in a file
 * @param lispProgramPath : The path to the lisp program
 * @return : Exit status
 */
int run(const char* lispProgramPath);

#endif
