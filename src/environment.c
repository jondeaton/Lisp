/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the lisp environment
 */

#include "environment.h"
#include <list.h>
#include <parser.h>
#include <string.h>
#include <math.h>
#include <lisp-objects.h>

// Static function declarations
static bool pair_matches_key(const obj *pair, const obj *key);
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

  obj* cdr = make_environment(primitive_names + 1, primitive_list + 1);
  return new_list_set(pair, cdr);
}

obj* make_pair(const obj *key, const obj *value) {
  obj* second = new_list_set(copy_recursive(value), NULL);
  return new_list_set(copy_recursive(key), second);
}

obj* lookup(const obj* o, const obj* env) {
  obj** entry = lookup_entry(o, env);
  return entry ? *entry : NULL;
}

obj** lookup_entry(const obj* key, const obj* env) {
  obj* pair = lookup_pair(key, env);
  if (pair == NULL) return NULL;
  return &list_of(list_of(pair)->cdr)->car;
}

obj* lookup_pair(const obj* key, const obj* env) {
  if (key == NULL || env == NULL) return NULL;
  if (!is_list(env) || !is_atom(key))  return NULL;  // Environment should be a list, key should be atom

  obj* pair = list_of(env)->car;
  if (pair_matches_key(pair, key)) return pair;
  return lookup_pair(key, list_of(env)->cdr);
}

/**
 * Function: pair_matches_key
 * ----------------------------
 * Determines if a key-value pair has the specified key
 * @param pair: The pair to look for the key in
 * @param key: The key to look for in the pair
 * @return: True if the key in the pair is equal to the specified key
 */
static bool pair_matches_key(const obj *pair, const obj *key) {
  obj* pair_key = ith(pair, 0);
  return compare(pair_key, key);
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
