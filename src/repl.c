/*
 * File: run_lisp.c
 * ----------------
 * Presents the implementation of the lisp file runner
 */

#include <repl.h>
#include <interpreter.h>
#include <stack-trace.h>

#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include  <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

static bool check_read_permissions(const char* path);
static void int_handler(int signal);

static LispInterpreter* interpreter;

int run_lisp(const char *bootstrap_path, const char *program_file, bool run_repl,
             const char *history_file, bool verbose) {

  if (bootstrap_path && !check_read_permissions(bootstrap_path)) return errno;
  if (program_file && !check_read_permissions(program_file)) return errno;

  if (verbose) LOG_MSG("Initializing interpreter.");

  LispInterpreter interpreter;
  bool success = interpreter_init(&interpreter);
  if (!success) {
    LOG_ERROR("Error initializing interpreter");
    return -1;
  }

  signal(SIGINT, int_handler); // install signal handler
  if (bootstrap_path != NULL) {
    if (verbose) LOG_MSG("Interpreting library: %s", bootstrap_path);
    interpret_program(&interpreter, bootstrap_path, verbose);
  }

  if (program_file != NULL) {
    if (verbose) LOG_MSG("Running script: %s", program_file);
    interpret_program(&interpreter, program_file, verbose);
  }

  if (run_repl) {
    if (verbose) LOG_MSG("Running interactive interpreter.");
    if (verbose) LOG_MSG("History: %s", history_file);

    if (history_file != NULL) {
      int err = read_history(history_file);
      if (err) {
        if (err == ENOENT && verbose) LOG_MSG("History file not found");
        else perror(history_file);
      }
    }

    interpret_fd(&interpreter, stdin, stdout, verbose);

    if (history_file) {
      int err = write_history(history_file);
      if (err) perror(history_file);
    }
  }

  if (verbose) LOG_MSG("Disposing of interpreter.");
  interpreter_dispose(&interpreter);

  return 0; // success
}

/**
 * Function: int_handler
 * ---------------------
 * Handles keyboard interrupts so that garbage is collected and history is saved
 * if the user quits the program with a keyboard interrupt
 * @param signal: The signal to exit the program
 */
static void int_handler(int signal) {
  interpreter_dispose(interpreter);
  exit(signal);
}

/**
 * Function: check_read_permissions
 * --------------------------------
 * Checks if a file may be read, printing errors to standard error if
 * the file may not be read or does not exist.
 * @return: True if the file can be read, false otherwise
 */
static bool check_read_permissions(const char* path) {
  if (access(path, R_OK) != 0) {
    perror(path);
    return false;
  }
  return true;
}
