/*
 * File: main.c
 * ------------
 * Exports the entry point to Lisp
 *
 * Running a lisp program file:
 *  ./lisp myprog.lisp
 *
 * Running a the lisp interpreter
 *  ./lisp
 */

#include <repl.h>
#include <run_lisp.h>

static const char const* lispBootstrapPath = "../bootstrap.lisp";

int main(int argc, char* argv[]) {
  run(lispBootstrapPath);
  if (argc == 1) repl();
  else run(argv[1]);
}