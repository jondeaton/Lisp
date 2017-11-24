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

static bool check_read_permissions(const char* path);

int run_lisp(const char* bootstrap_path, const char* program_file, bool run_repl) {

  if (bootstrap_path && check_read_permissions(bootstrap_path) != 0) return errno;
  if (program_file && check_read_permissions(program_file)) return errno;

  repl_init();
  if (bootstrap_path) repl_run_program(bootstrap_path);
  if (program_file) repl_run_program(program_file);
  if (run_repl) repl_run();
  repl_dispose();
  return 0; // success
}

/**
 * Function: check_read_permissions
 * --------------------------------
 * Checks if a file may be read
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
