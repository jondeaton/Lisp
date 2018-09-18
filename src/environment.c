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
#include <assert.h>

struct environment {
  CMap *global_scope;
  struct CVector *stack;
};

static int cmp_atoms(const obj **ap, const obj **bp) {
  assert(ap != NULL);
  assert(bp != NULL);
  return strcmp(ATOM(*ap), ATOM(*bp));
}

static unsigned int hash_atom(const obj **ap, size_t keysize UNUSED) {
  assert(ap != NULL);
  return atom_hash(*ap, keysize);
}


static CMap *new_scope() {
  return cmap_create(sizeof(obj*), sizeof(obj*),
                     (CMapHashFn) hash_atom, (CmpFn) cmp_atoms,
                     NULL, NULL, 0);;
}

struct environment *new_environment() {
  struct environment *env = malloc(sizeof(struct environment));
  if (env == NULL) return NULL;

  env->stack = new_cvec(sizeof(CMap *), 0, (CleanupFn) cmap_dispose);
  if (env->stack == NULL) {
    free(env);
    return NULL;
  }

  env->global_scope = new_scope();
  if (env->global_scope == NULL) {
    free(env->stack);
    free(env);
    return NULL;
  }

  return env;
}

bool def_global(struct environment *env, const obj *name, const obj *value) {
  assert(env != NULL);
  assert(name != NULL);
  void *kv = cmap_insert(env->global_scope, &name, &value);
  return kv != NULL;
}

bool def_local(struct environment *env, const obj *name, const obj *value) {
  assert(env != NULL);
  assert(name != NULL);

  CMap **scope = cvec_last(env->stack);
  if (scope == NULL) return false;

  void *kv = cmap_insert(*scope, &name, &value);
  return kv != NULL;
}


bool push_scope(struct environment *env) {
  assert(env != NULL);
  CMap *scope = new_scope();
  if (scope == NULL) return false;
  cvec_append(env->stack, &scope);
  return true;
}

void pop_scope(struct environment *env) {
  assert(env != NULL);
  cvec_pop(env->stack);
}

obj *env_lookup(struct environment *env, const obj *name) {
  assert(env != NULL);
  assert(name != NULL);

  obj **value;
  if (cvec_count(env->stack) > 0) {
    CMap **scope = cvec_last(env->stack);
    assert(scope != NULL);
    value = cmap_lookup(*scope, &name);
    if (value != NULL) return *value;
  }
  value = cmap_lookup(env->global_scope, &name);
  if (value == NULL) return NULL;
  return *value;
}


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
    obj *second = new_list_set(value, NULL);
    return new_list_set(key, second);
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
