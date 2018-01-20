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

#include <repl.h>
#include <run-lisp.h>
#include <unistd.h>
#include <stdio.h>

static void parse_command_line_args(int argc, char* argv[]);
static void print_version_information();

const char* optstring = ":rb:t:v";
char const* bootstrap_path = NULL;
char const* program_path = NULL;
char const* history_file = "~/.lisp-history";
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
static void parse_command_line_args(int argc, char* argv[]) {
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
        case 't': {
          history_file = optarg;
          break;
        }
        case 'v': {
          print_version_information();
          exit(0);
          break;
        }
        default: break;
      }
    } else {
      if (optind < argc) program_path = argv[optind];
    }
  }
}

/**
 * Function: print_version_information
 * -----------------------------------
 * Prints the version information for the Lisp interpreter.
 */
static void print_version_information() {
  printf("Lisp Interpreter 1.0\n"
           "Author: Jon Deaton, January 2018\n");
}
