/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include "environment.h"
#include <list.h>
#include <parser.h>
#include <string.h>
#include <math.h>

// Static function declarations
static obj* append_environments(obj* env1, const obj* env2);

obj* init_env() {
  obj* prim_env = get_primitive_library();
  obj* math_env = get_math_library();
  obj* env = append_environments(math_env, prim_env);
  return env;
}

obj* make_environment(atom_t const primitive_names[], const primitive_t primitive_list[]) {
  if (primitive_names[0] == NULL || primitive_list[0] == NULL) return NULL;

  obj* key = new_atom(primitive_names[0]);
  obj* value = new_primitive(primitive_list[0]);
  obj* pair = make_pair(key, value);

  obj* env = new_list();
  list_of(env)->car = pair;
  list_of(env)->cdr = make_environment(primitive_names + 1, primitive_list + 1);
  return env;
}

obj* make_pair(const obj* name, const obj* value) {
  obj* first_item = new_list();
  list_of(first_item)->car = copy(name);

  obj* second_item = new_list();
  list_of(second_item)->car = copy(value);

  list_of(first_item)->cdr = second_item;

  return first_item;
}


obj** lookup_entry(const obj* o, const obj* env) {
  if (o == NULL || env == NULL) return NULL;

  // Error: The environment should be a list
  if (env->objtype != list_obj) return NULL;

  // Error: Can't lookup something that isn't an atom
  if (o->objtype != atom_obj) return NULL;

  // Get the list
  obj* pair = list_of(env)->car;

  if (strcmp(atom_of(o), atom_of(list_of(pair)->car)) == 0)
    return &list_of(list_of(pair)->cdr)->car;

  else return lookup_entry(o, list_of(env)->cdr);
}


obj* lookup(const obj* o, const obj* env) {
  obj** entry = lookup_entry(o, env);
  return entry ? *entry : NULL;
}

/**
 * Function: append_environments
 * -----------------------------
 * Prepends one environment to another
 * @param env1: The environment to prepend to env2
 * @param env2: The environment to append to env1
 * @return: env1 except now the last element points to env2
 */
static obj* append_environments(obj* env1, const obj* env2) {
  if (list_of(env1)->cdr == NULL) list_of(env1)->cdr = (obj*) env2;
  else append_environments(list_of(env1)->cdr, env2);
  return env1;
}
