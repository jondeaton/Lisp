/*
 * File: math.h
 * ------------
 * Presents the interface of the math library
 */

#ifndef _LISP_MATH_H_INCLUDED
#define _LISP_MATH_H_INCLUDED

#include "interpreter.h"
#include "list.h"
#include "garbage-collector.h"

obj* get_math_library(void);

obj *add(const obj *args, LispInterpreter *interpreter);
obj *sub(const obj *o, LispInterpreter *interpreter);
obj *mul(const obj *o, LispInterpreter *interpreter);
obj *divide(const obj *o, LispInterpreter *interpreter);
obj *mod(const obj *o, LispInterpreter *interpreter);
obj *equal(const obj *o, LispInterpreter *interpreter);
obj *gt(const obj *o, LispInterpreter *interpreter);
obj *gte(const obj *o, LispInterpreter *interpreter);
obj *lt(const obj *o, LispInterpreter *interpreter);
obj *lte(const obj *o, LispInterpreter *interpreter);

#endif // _LISP_MATH_H_INCLUDED
