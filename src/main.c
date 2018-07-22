/*
 * File: main.c
 * ------------
 * Exports the entry point to Lisp Interpreter
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>

#include <repl.h>

#define HISTORY_FILE_LENGTH 128
#define DEFAULT_HISTORY_FILE ".lisp-history"

/**
 * @struct InterpreterConfig holds command line options for Lisp interpreter.
 */
struct InterpreterConfig {
  char const *bootstrap_path;
  char const *program_path;
  bool run_repl;
  bool verbose;
  char history_buffer[HISTORY_FILE_LENGTH];
  char *history_file;
};

static void parse_command_line_args(int argc, char* argv[], struct InterpreterConfig *config);
static void print_version_information();

const char *const optstring = ":rb:t:vh";

// If not history file was specified on CLI, then get it from home directory
static void set_history_file(struct InterpreterConfig *config) {
  if (config->history_file == NULL) {
    const char* home_dir = getpwuid(getuid())->pw_dir;

    int num_written = snprintf(config->history_buffer, 
        sizeof(config->history_buffer),
        "%s/%s", home_dir, DEFAULT_HISTORY_FILE);

    if (num_written < 0) {
      config->history_file = NULL;
      printf("Default history file not found. Interpreter history disabled.");
    }
    else config->history_file = config->history_buffer;
  }
}

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
  
  struct InterpreterConfig config;
  parse_command_line_args(argc, argv, &config);
  set_history_file(&config);
  
  return run_lisp(config.bootstrap_path, config.program_path,
                  config.run_repl, config.history_file, config.verbose);
}

/**
 * Function: parse_command_line_args
 * ---------------------------------
 * Parses the command line arguments and sets the relevant config variabless
 * @param argc Argument count
 * @param argv Argument array
 * @param config configuration struct
 */
static void parse_command_line_args(int argc, char* argv[],
                                    struct InterpreterConfig *config) {

  // set defaults
  config->bootstrap_path = NULL;
  config->program_path = NULL;
  config->run_repl = true;
  config->verbose = false;
  config->history_file = NULL;

  bool repl_flag = false;
  while (optind < argc) {
    char c;
    if ((c = getopt(argc, argv, optstring)) != -1) {
      switch(c) {
        case 'r': {
          repl_flag = true;
          break;
        }
        case 'b': {
          config->bootstrap_path = optarg;
          break;
        }
        case 't': {
          config->history_file = optarg;
          break;
        }
        case 'v': {
          config->verbose = true;
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
        config->program_path = argv[optind];
        config->run_repl = repl_flag;
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
  printf("Lisp Interpreter 1.0, STD %ld\n"
         "Author: Jon Deaton, %s %s\n%s\n",
         __STDC_VERSION__, __DATE__, __TIME__, __VERSION__);
}
