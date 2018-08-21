/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the lisp environment
 */

#include <environment.h>
#include <lisp-objects.h>
#include <list.h>
#include <math-lib.h>
#include <parser.h>
#include <string.h>

// Static function declarations
static bool pair_matches_key(const obj *pair, const obj *key);

obj* init_env() {
  obj* prim_env = get_primitive_library();
  obj* math_env = get_math_library();
  obj* env = join_lists(math_env, prim_env);
  return env;
}

obj* create_environment(atom_t const *primitive_names, primitive_t const *primitive_list) {
  if (primitive_names[0] == NULL || primitive_list[0] == NULL) return NULL;

  obj* key = new_atom(primitive_names[0]);
  obj* value = new_primitive(primitive_list[0]);
  obj* pair = make_pair(key, value, false);

  obj* cdr = create_environment(primitive_names + 1, primitive_list + 1);
  return new_list_set(pair, cdr);
}

obj *make_pair(obj *key, obj *value, bool copy) {
  if (copy) {
    obj *second = new_list_set(copy_recursive(value), NULL);
    return new_list_set(copy_recursive(key), second);
  } else {
    obj* second = new_list_set(value, NULL);
    return new_list_set(key, second);
  }
}

obj* lookup(const obj* o, const obj* env) {
  obj** entry = lookup_entry(o, env);
  return entry ? *entry : NULL;
}

obj** lookup_entry(const obj* key, const obj* env) {
  obj* pair = lookup_pair(key, env);
  if (pair == NULL) return NULL;
  return & CAR(CDR(pair));
}

obj* lookup_pair(const obj* key, const obj* env) {
  if (key == NULL || env == NULL) return NULL;
  if (!is_list(env) || !is_atom(key))  return NULL;  // Environment should be a list, key should be atom

  obj* pair = CAR(env);
  if (pair_matches_key(pair, key)) return pair;
  return lookup_pair(key, CDR(env));
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
