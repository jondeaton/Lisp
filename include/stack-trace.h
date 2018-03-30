/*
 * File: stack-trace.h
 * -------------------
 * Presents the interface of stack trace operations
 */

#ifndef _STACK_TRACE_H_INCLUDED
#define _STACK_TRACE_H_INCLUDED

#include "lisp-objects.h"
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>

/*
 * Variadic macro for easy printf-style error logging:
 *    LOG_ERROR("aw snap!");
 *    LOG_ERROR("Error code: %d", 42);
 */
#define LOG_ERROR(...) log_error(__func__, __VA_ARGS__)
#define LOG_MSG(...) log_message(__func__, __VA_ARGS__)

// Use of malloc is prevalent - this directive helps make cleaner code
#define CHECK_NARGS(args, expected) check_nargs(__func__, args, expected)
#define CHECK_NARGS_MIN(args, minimum) check_nargs_min(__func__, args, minimum)
#define CHECK_NARGS_MAX(args, maximum) check_nargs_max(__func__, args, maximum)
#define MALLOC_CHECK(o) if ((o) == NULL) { perror("malloc"); exit(ENOMEM); }

/**
 * Function: log_error
 * -------------------
 * Prints an error message to standard error in red. Meant to be used for stack tracing
 *
 * Suggested usage:
 *
 *      log_error(__func__, "something bad happened! Error code: %d", 42);
 *
 * which would yield the following to be printed to standard error:
 *
 *      [invoking_function_name]: "something bad happened! Error code: 42"
 *
 * Alternatively, the macros defined above can be used for automatic substitution of __func__ like so:
 *
 *      LOG_ERROR("Expecting %d, saw %d", expected_num, observed_num);
 *
 * which produces the same format of error message as above.
 * @param context: The context in which the error occurred (will be placed between brackets)
 * @param message_format: The format of the error message content
 * @param ... (additional arguments): A sequence of additional arguments, each containing a value to be used to
 * replace a format specifier in the message_format string. There should be at least as many of these
 * arguments as the number of values specified in the format specifiers. Additional arguments are ignored
 * by the function.
 * @return: NULL pointer
 */
void* log_error(const char* context, const char* message_format, ...);

/**
 * Function: log_message
 * ---------------------
 * Prints a message to standard error in blue.
 * @param context: The context in which the message is relevant.
 * @param message_format: The format of the message content
 * @param ... (additional arguments): A sequence of additional arguments, each containing a value to be used to
 * replace a format specifier in the message_format string. There should be at least as many of these
 * arguments as the number of values specified in the format specifiers. Additional arguments are ignored
 * by the function.
 */
void log_message(const char* context, const char* message_format, ...);

/**
 * Function: check_nargs
 * ---------------------
 * Checks if the number of arguments is equal to the number that are expected, logging
 * an informative error if this is not the case, and returning false in that case. If there
 * are the expected number of arguments, then nothing is printed and the function returns true
 * @param context: The context in which the arguments are being checked
 * @param args: The arguments provided to the primitive
 * @param expected: The number of arguments expected to be present in the list
 * @return: True if the number of arguments is equal to the number expected, false otherwise
 */
bool check_nargs(const char *context, const obj *args, int expected);

/**
 * Function: check_nargs_min
 * -------------------------
 * Determines if a sufficient number of arguments are present, logging an informative error if there
 * are fewer arguments than the specified minimum number of arguments.
 * @param context: The context in which the arguments are being checked
 * @param args: The arguments to check
 * @param minimum: The minimum allowable number of arguments
 * @return: True if the number of arguments found is greater than or equal to the minimum allowable
 * number, and false otherwise
 */
bool check_nargs_min(const char *context, const obj *args, int minimum);

/**
 * Function: check_nargs_max
 * -------------------------
 * Determines if the number of arguments does not exceed some maximum number, logging an informative error
 * if there are more than the specified maximum number of arguments.
 * @param context: The context in which the arguments are being checked
 * @param args: The arguments to check
 * @param maximum: The maximum number of arguments allowable
 * @return: True if there are fewer than or equal to the maximum number of arguments, false otherwise.
 */
bool check_nargs_max(const char* context, const obj *args, int maximum);

#endif // _STACK_TRACE_H_INCLUDED
