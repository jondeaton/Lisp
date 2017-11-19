/*
 * File: stack-trace.c
 * -------------------
 * Presents the implementation of stack trace logging for the lisp interpreter
 */
#include <stdio.h>

#define KRED  "\x1B[31m"
#define RESET "\033[0m"


void log_error(const char* context, const char* message) {
  fprintf(stderr, KRED "\t[%s]: %s\n" RESET, context, message);
}
