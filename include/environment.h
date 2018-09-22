/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _ENVIRONMENT_H_INCLUDED
#define _ENVIRONMENT_H_INCLUDED

#include "primitives.h"
#include <cmap.h>
#include <cvector.h>

#include <stdbool.h>

struct environment {
  Map global_scope;
  struct CVector stack;
};

struct environment *new_environment();
bool init_env(struct environment *env);
bool def_global(struct environment *env, const obj *name, const obj *value);
bool def_local(struct environment *env, const obj *name, const obj *value);
bool push_scope(struct environment *env);
void pop_scope(struct environment *env);
obj *env_lookup(struct environment *env, const obj *name);
void env_dispose(struct environment *env);

#endif // _ENVIRONMENT_H_INCLUDED
