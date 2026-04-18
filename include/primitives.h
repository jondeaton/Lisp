/*
 * File: primitives.h
 * ------------------
 * Presents the interface to the lisp primitives.
 */

#ifndef _LISP_PRIMITIVES_H_INCLUDED
#define _LISP_PRIMITIVES_H_INCLUDED

#include "lisp-objects.h"
#include "garbage-collector.h"
#include "interpreter.h"

#define def_primitive(name) obj *name(const obj *args UNUSED, LispInterpreter *interpreter UNUSED)

obj *get_primitive_library(void);
obj *new_primitive(primitive_t primitive);
obj *t(GarbageCollector *gc);
obj *nil(GarbageCollector *gc);

#endif // _LISP_PRIMITIVES_H_INCLUDED
