/*
 * File: run_lisp.c
 * ----------------
 * Presents the implementation of the lisp file runner
 */

#include <run-lisp.h>
#include <repl.h>
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

int run_lisp(const char* bootstrap_path, const char* program_file, bool run_repl) {

  if (bootstrap_path && check_read_permissions(bootstrap_path) != 0) return errno;
  if (program_file && check_read_permissions(program_file)) return errno;

  int err = read_history(LISP_HISTORY);
  if (err && err != ENOENT) perror(LISP_HISTORY);

  repl_init();
  signal(SIGINT, int_handler);
  if (bootstrap_path) repl_run_program(bootstrap_path);
  if (program_file) repl_run_program(program_file);
  if (run_repl) repl_run();
  repl_dispose();

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
  repl_dispose();
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
    if (errno == ENOENT) fprintf(stderr, "No such file: %s\n", path);
    if (errno == EACCES) fprintf(stderr, "Permission denied: %s\n", path);
    return false;
  }
  return true;
}
