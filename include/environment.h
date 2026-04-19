/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _ENVIRONMENT_H_INCLUDED
#define _ENVIRONMENT_H_INCLUDED

#include "primitives.h"

obj* init_env(void);
obj* create_environment(atom_t const *primitive_names, primitive_t const *primitive_list);
obj* lookup(const obj* o, const obj* env);
obj** lookup_entry(const obj* key, const obj* env);
obj* lookup_pair(const obj* key, const obj* env);
obj *make_pair(obj *key, obj *value);

#endif // _ENVIRONMENT_H_INCLUDED
