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
 *  ./lisp my-program.lisp
 *
 * Run lisp program file and then the repl
 *  ./lisp my-program.lisp -r
 *
 * Run lisp interpreter with specific bootstrap file
 *  ./lisp -b my-bootstrap.lisp
 */

#include "repl.h"
#include "run-lisp.h"
#include <unistd.h>

void parse_command_line_args(int argc, char* argv[]);

const char* optstring = ":rb:";
char const* bootstrap_path = NULL;
char const* program_path = NULL;
bool run_repl = true;

/**
 * Entry Point: main
 * -----------------
 * Lisp interpreter entry point. Parses command line arguments
 * and runts the lisp interpreter either either through an interactive
 * prompt or on a specified file.
 * @param argc: Argument count
 * @param argv: Argument array
 * @return: Exit status
 */
int main(int argc, char* argv[]) {
  parse_command_line_args(argc, argv);
  return run_lisp(bootstrap_path, program_path, run_repl);
}

/**
 * Function: parse_command_line_args
 * ---------------------------------
 * Parses the command line arguments and sets the relevant global variables
 * @param argc; Argument count
 * @param argv: Argument array
 */
void parse_command_line_args(int argc, char* argv[]) {
  while (optind < argc) {
    int c;
    if ((c = getopt(argc, argv, optstring)) != -1) {
      switch(c) {
        case 'r': {
          run_repl = true;
          break;
        }
        case 'b': {
          bootstrap_path = optarg;
          break;
        }
        default: break;
      }
    } else {
      if (optind < argc) program_path = argv[optind];
    }
  }
}