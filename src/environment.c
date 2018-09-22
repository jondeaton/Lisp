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



static int cmp_atoms(const obj **ap, const obj **bp) {
  assert(ap != NULL);
  assert(bp != NULL);
  return strcmp(ATOM(*ap), ATOM(*bp));
}

static unsigned int hash_atom(const obj **ap, size_t keysize UNUSED) {
  assert(ap != NULL);
  return atom_hash(*ap, keysize);
}


static bool init_scope(Map *scope) {
  assert(scope != NULL);
  return cmap_init(scope, sizeof(obj *), sizeof(obj *),
            (CMapHashFn) hash_atom, (CmpFn) cmp_atoms,
            NULL, NULL, 0);
}

struct environment *new_environment() {
  struct environment *env = malloc(sizeof(struct environment));
  if (env == NULL) return NULL;
  bool success = init_env(env);
  if (!success) {
    free(env);
    return NULL;
  }
  return env;
}

bool init_env(struct environment *env) {
  assert(env != NULL);

  bool success = cvec_init(&env->stack, sizeof(Map), 0, (CleanupFn) cmap_dispose)
  if (!success) return false;

  success = init_scope(&env->global_scope);
  if (!success) {
    cvec_dispose(&env->stack);
    return false;
  }
  return success;
}

bool def_global(struct environment *env, const obj *name, const obj *value) {
  assert(env != NULL);
  assert(name != NULL);
  void *kv = cmap_insert(&env->global_scope, &name, &value);
  return kv != NULL;
}

bool def_local(struct environment *env, const obj *name, const obj *value) {
  assert(env != NULL);
  assert(name != NULL);

  Map **scope = cvec_last(&env->stack);
  if (scope == NULL) return false;

  void *kv = cmap_insert(*scope, &name, &value);
  return kv != NULL;
}


bool push_scope(struct environment *env) {
  assert(env != NULL);
  Map *scope = init_scope();
  if (scope == NULL) return false;
  cvec_append(&env->stack, &scope);
  return true;
}

void pop_scope(struct environment *env) {
  assert(env != NULL);
  cvec_pop(&env->stack);
}

obj *env_lookup(struct environment *env, const obj *name) {
  assert(env != NULL);
  assert(name != NULL);

  obj **value;
  if (cvec_count(&env->stack) > 0) {
    Map **scope = cvec_last(&env->stack);
    assert(scope != NULL);
    value = cmap_lookup(*scope, &name);
    if (value != NULL) return *value;
  }
  value = cmap_lookup(&env->global_scope, &name);
  if (value == NULL) return NULL;
  return *value;
}

void env_dispose(struct environment *env) {
  assert(env != NULL);
  cmap_dispose(&env->global_scope);
  cvec_dispose(&env->stack);
}
