/*
 * File: stack-trace.c
 * -------------------
 * Presents the implementation of stack trace logging for the lisp interpreter
 */
#include <stack-trace.h>
#include <stdio.h>
#include <list.h>
#include <stdarg.h>

#define KRED  "\x1B[31m"
#define KBLU  "\x1B[34m"
#define RESET "\033[0m"

#define ERROR_BUFFER_SIZE 256
char err_buff[ERROR_BUFFER_SIZE];

void *log_error(const char* context, const char* message_format, ...) {
  va_list ap;
  va_start(ap, message_format);
  int n = vsnprintf(err_buff, ERROR_BUFFER_SIZE, message_format, ap); // substitute var args into message
  if (n > 0)
    fprintf(stderr, KRED "\t[%s]: %s\n" RESET, context, err_buff); // substitute message into error log
  va_end(ap);
  return NULL; // Return NULL for cleaner NULL-returning error handling
}

void log_message(const char* context, const char* message_format, ...) {
  va_list ap;
  va_start(ap, message_format);
  int n = vsnprintf(err_buff, ERROR_BUFFER_SIZE, message_format, ap); // substitute var args into message
  if (n > 0)
    fprintf(stdout, KBLU "\t[%s]: %s\n" RESET, context, err_buff); // substitute message into error log
  va_end(ap);
}

bool check_nargs(const char *context, const obj *args, int expected) {
  int nargs = list_length(args);
  if (nargs == expected) return true;
  log_error(context, "Expected %d arguments, got %d", expected, nargs);
  return false;
}

bool check_nargs_min(const char *context, const obj *args, int minimum) {
  int nargs = list_length(args);
  if (nargs >= minimum) return true;
  log_error(context, "Expected %d or more arguments, got %d", minimum, nargs);
  return false;
}

bool check_nargs_max(const char* context, const obj *args, int maximum) {
  int nargs = list_length(args);
  if (nargs <= maximum) return true;
  log_error(context, "Expected %d or fewer arguments, got %d", maximum, nargs);
  return false;
}
