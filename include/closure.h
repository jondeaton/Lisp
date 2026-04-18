/*
 * File: closure.h
 * ---------------
 * Closure-related functionality
 */

#ifndef _CLOSURE_H_INCLUDED
#define _CLOSURE_H_INCLUDED

#include "lisp-objects.h"
#include "garbage-collector.h"
#include "interpreter.h"

obj *closure_partial_application(const obj *closure, const obj *args, LispInterpreter *interpreter);
obj *new_closure_set(obj *params, obj *procedure, obj *captured);
obj *associate(obj *names, const obj *args, LispInterpreter *interpreter, bool eval_args);

#endif // _CLOSURE_H_INCLUDED
