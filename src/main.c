/*
 * File: main.c
 * ------------
 * Presents the entry point to Lisp
 *
 * Usage:
 *
 * ./lisp myprog.lisp
 *
 * ./lisp
 *
 */
#include "repl.h"

int main(int argc, char* argv[]) {
  if (argc == 1) repl();
  else repl();
}
