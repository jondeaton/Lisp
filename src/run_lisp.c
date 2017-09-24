/*
 * File: run_lisp.c
 * ----------------
 * Presents the implementation of the lisp file runner
 */

#include <run_lisp.h>
#include <environment.h>
#include <evaluator.h>
#include <repl.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>

int run(const char* lispProgramPath, obj* env) {

  // Error checking
  if (access(lispProgramPath, R_OK) != 0) {
    if (errno == ENOENT) fprintf(stderr, "No such file: %s\n", lispProgramPath);
    if (errno == EACCES) fprintf(stderr, "Permissions denied: %s\n", lispProgramPath);
    return errno;
  }

  FILE* fd = fopen(lispProgramPath, O_RDONLY);

  if (env == NULL) env = initEnv();
  while (!feof(fd)) eval(readExpression(fd, NULL, NULL), env);
  return 0;
}