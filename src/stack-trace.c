/*
 * File: stack-trace.c
 * -------------------
 * Presents the implementation of stack trace logging for the lisp interpreter
 */
#include <stack-trace.h>
#include <stdio.h>
#include <list.h>

#define KRED  "\x1B[31m"
#define RESET "\033[0m"

void log_error(const char* context, const char* message) {
  fprintf(stderr, KRED "\t[%s]: %s\n" RESET, context, message);
}

bool check_num_args(const char* context, const obj* args, int expected) {
  int nargs = list_length(args);
  if (nargs == expected) return true;
  char err_msg[ERR_BUFF_SIZE];
  sprintf(err_msg, "Expected %d arguments, got %d", expected, nargs);
  log_error(context, err_msg);
  return false;
}

bool check_num_args_gt(const char* context, const obj* args, int minimum) {
  int nargs = list_length(args);
  if (nargs >= minimum) return true;
  char err_msg[ERR_BUFF_SIZE];
  sprintf(err_msg, "Expected %d or more arguments, got %d", minimum, nargs);
  log_error(context, err_msg);
  return false;
}
