/*
 * File: stack-trace.h
 * -------------------
 * Presents the interface of stack trace operations
 */

#ifndef _STACK_TRACE_H_INCLUDED
#define _STACK_TRACE_H_INCLUDED

#include <lisp-objects.h>
#include <stdbool.h>

// Buffer to use to store error messages
#define ERR_BUFF_SIZE 256

// Use of malloc is prevalent - this directive helps make cleaner code
#define LOG_MALLOC_FAIL log_error(__func__, "Memory allocation failure")

/**
 * Function: log_error
 * -------------------
 * Prints an error message to standard error in red. Meant to be used for stack tracing
 * Suggested usage: log_error(__func__, "something bad happened!")
 * @param context: The context in which the error occurred
 * @param message: The message to print
 */
void log_error(const char* context, const char* message);

/**
 * Function: check_num_args
 * ------------------------
 * Checks if the number of arguments is equal to the number that are expected, logging
 * an informative error if this is not the case, and returning false in that case. If there
 * are the expected number of arguments, then nothing is printed and the function returns true
 * @param context: The context in which the arguments are being checked
 * @param args: The arguments provided to the primitive
 * @param expected: The number of arguments expected to be present in the list
 * @return: True if the number of arguments is equal to the number expected, false otherwise
 */
bool check_num_args(const char* context, const obj* args, int expected);

/**
 * Function: check_num_args_gt
 * ---------------------------
 * Determines if a sufficient number of arguments are present, logging an informative error if there
 * are fewer arguments than the specified minimum number
 * @param context: The context in which the arguments are being checked
 * @param args: The arguments to check
 * @param minimum: The minimum allowable number of arguments
 * @return: True if the number of arguments found is greater than or equal to the minimum allowable
 * number, and false otherwise
 */
bool check_num_args_gt(const char* context, const obj* args, int minimum);

#endif // _STACK_TRACE_H_INCLUDED
