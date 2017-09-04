/*
 * File: main.c
 * ------------
 * Exports the entry point to Lisp
 *
 * Command line usage
 *
 * Run lisp interpreter:
 *  ./lisp
 *
 * Run lisp program file:
 *  ./lisp myprog.lisp
 *
 */

#include <repl.h>
#include <run_lisp.h>

static const char* lispBootstrapPath = "../bootstrap.lisp";

/**
 * Entry Point: main
 * -----------------
 * Bootstraps the Lisp interpreter using the meta-circular
 * evaluator located in the bootstrap lisp program
 * @param argc : Number of command line arguments
 * @param argv : NULL terminated list of command line arguments
 * @return : Exit status
 */
int main(int argc, char* argv[]) {
//  run(lispBootstrapPath);
  if (argc == 1) return repl();
  else return run(argv[1]);
}