/*
 * File: closures.c
 * ----------------
 * Presents the implementation of closure related functionality
 */

#include <lisp-objects.h>
#include "closure.h"
#include "garbage-collector.h"
#include "list.h"

// Static function declarations
obj* make_closure_init(obj* procedure, obj* env);
static obj* get_body(const obj* lambda);

obj* make_closure(const obj* lambda, obj** envp) {
  obj* procesure = copy_recursive(get_body(lambda));
  obj* env = copy_recursive(*envp);
  obj* closure = make_closure_init(procesure, env);
  add_allocated(closure);
  return closure;
}

obj* make_closure_init(obj* procedure, obj* env) {
  obj* o = new_closure();
  closure_of(o)->procedure = procedure;
  closure_of(o)->env = env;
  return o;
}

static obj* get_body(const obj* lambda) {
  return ith(lambda, 1);
}