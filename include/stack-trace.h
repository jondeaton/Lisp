/*
 * File: stack-trace.h
 * -------------------
 * Presents the interface of stack trace operations
 */

#ifndef _STACK_TRACE_H_INCLUDED
#define _STACK_TRACE_H_INCLUDED

// Buffer to use to store error messages
#define ERR_BUFF_SIZE 256

// Use of malloc is prevalent - this directive help make cleaner code
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

#endif // _STACK_TRACE_H_INCLUDED
