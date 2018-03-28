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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>

static void parse_command_line_args(int argc, char* argv[]);
static void print_version_information();

const char *const optstring = ":rb:t:v";
char const* bootstrap_path = NULL;
char const* program_path = NULL;
bool run_repl = true;
bool verbose = false;

#define HISTORY_FILE_LENGTH 128
char history_buffer[HISTORY_FILE_LENGTH];
#define DEFAULT_HISTORY_FILE ".lisp-history"

const char* history_file = NULL;

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
  if (!history_file) {
    const char* home_dir = getpwuid(getuid())->pw_dir;
    strcpy(history_buffer, home_dir);
    strcat(history_buffer, "/");
    strcat(history_buffer, DEFAULT_HISTORY_FILE);
    history_file = history_buffer;
  }
  return run_lisp(bootstrap_path, program_path, run_repl, history_file, verbose);
}

/**
 * Function: parse_command_line_args
 * ---------------------------------
 * Parses the command line arguments and sets the relevant global variables
 * @param argc; Argument count
 * @param argv: Argument array
 */
static void parse_command_line_args(int argc, char* argv[]) {
  bool repl_flag = false;
  while (optind < argc) {
    int c;
    if ((c = getopt(argc, argv, optstring)) != -1) {
      switch(c) {
        case 'r': {
          repl_flag = true;
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
          verbose = true;
          break;
        }
        case 'h': {
          print_version_information();
          exit(0);
        }
        default: break;
      }
    } else {
      if (optind < argc) {
        program_path = argv[optind];
        run_repl = repl_flag;
      }
      break;
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
           "Author: Jon Deaton, August 2017 - March 2018\n");
}
