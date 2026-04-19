/*
 * File: interpreter.h
 * -------------------
 * Presents the interface to the Lisp interpreter
 */

#ifndef _LISP_INTERPRETER_H_INCLUDED
#define _LISP_INTERPRETER_H_INCLUDED

#include "parser.h"
#include "garbage-collector.h"
#include <stdio.h>

struct LispInterpreter {
  obj* env;
  GarbageCollector gc;
};

bool interpreter_init(LispInterpreter *interpreter);
void interpret_program(LispInterpreter *interpreter, const char *program_file, bool verbose);
void interpret_fd(LispInterpreter *interpreter, FILE *fd_in, FILE *fd_out, bool verbose);
expression interpret_expression(LispInterpreter *interpreter, const_expression expr);
void interpreter_dispose(LispInterpreter *interpreter);

#endif //_LISP_INTERPRETER_H_INCLUDED
