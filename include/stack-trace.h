/*
 * File: stack-trace.h
 * -------------------
 * Presents the interface of stack trace operations
 */

#include <stdio.h>

#define KRED  "\x1B[31m"
#define RESET "\033[0m"

/**
 * Function: log_error
 * -------------------
 * Prints an error message to standard error in red. Meant to be used for stack tracing
 * Suggested usage: log_error(__func__, "something bad happened")
 * @param context: The context in which the error occurred
 * @param message: The message to print
 */
void log_error(const char* context, const char* message) {
  fprintf(stderr, KRED "%s: %s" RESET, context, message);
}