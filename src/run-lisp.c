/*
 * File: run_lisp.c
 * ----------------
 * Presents the implementation of the lisp file runner
 */

#include <run-lisp.h>
#include <interpreter.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include  <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#define LISP_HISTORY ".lisp-history"

static bool check_read_permissions(const char* path);
static void int_handler(int signal);

static LispInterpreter* interpreter;

int run_lisp(const char* bootstrap_path, const char* program_file, bool run_repl) {

  if (bootstrap_path && check_read_permissions(bootstrap_path) != 0) return errno;
  if (program_file && check_read_permissions(program_file)) return errno;

  int err = read_history(LISP_HISTORY);
  if (err && err != ENOENT) perror(LISP_HISTORY);

  interpreter = interpreter_init();
  signal(SIGINT, int_handler);
  if (bootstrap_path) interpret_program(interpreter, bootstrap_path);
  if (program_file) interpret_program(interpreter, program_file);
  if (run_repl) interpret_fd(interpreter);
  interpreter_dispose(interpreter);

  err = write_history(LISP_HISTORY);
  if (err) perror(LISP_HISTORY);

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
 * todo: change this to use perror!
 */
static bool check_read_permissions(const char* path) {
  if (access(path, R_OK) != 0) {
    if (errno == ENOENT) fprintf(stderr, "No such file: %s\n", path);
    if (errno == EACCES) fprintf(stderr, "Permission denied: %s\n", path);
    return false;
  }
  return true;
}
