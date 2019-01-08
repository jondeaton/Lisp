/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _ENVIRONMENT_H_INCLUDED
#define _ENVIRONMENT_H_INCLUDED

#include "lisp-objects.h"
#include <cmap.h>
#include <cvector.h>

#include <stdbool.h>

struct environment {
  struct Map global_scope;
  struct CVector stack;
};


struct environment *new_environment();
bool init_env(struct environment *env);
bool def_global(struct environment *env, const obj *name, const obj *value);
bool def_local(struct environment *env, const obj *name, const obj *value);
bool push_scope(struct environment *env);
void pop_scope(struct environment *env);
bool bind_closure(struct environment *env, const obj *closure, const obj *args);
obj *env_lookup(const struct environment *env, const obj *name);
void env_dispose(struct environment *env);

#endif // _ENVIRONMENT_H_INCLUDED
