/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <list.h>

#include <string.h>
#include <stdlib.h>

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "env", "defmacro", NULL };

static const primitive_t primitive_functions[] = {&quote, &atom, &eq, &car, &cdr, &cons,
                                                  &cond, &set, &env_prim, &defmacro,  NULL };

// Static function declarations
static obj *ith_arg_value(const obj *args, obj **envp, int i, GarbageCollector *gc);

obj* get_primitive_library() {
  return make_environment(primitive_reserved_names, primitive_functions);
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  MALLOC_CHECK(o);
  o->objtype = primitive_obj;
  memcpy(PRIMITIVE(o), &primitive, sizeof(primitive));
  return o;
}

//primitive_t* PRIMITIVE(const obj *o) {
//  return (primitive_t*) CONTENTS(o);
//}

// Allocate new truth atom
obj *t(GarbageCollector *gc) {
  obj* t = new_atom("t");
  gc_add(gc, t);
  return t;
}

// Allocate new empty list
obj *empty(GarbageCollector *gc) {
  obj* list = new_list_set(NULL, NULL);
  gc_add(gc, list);
  return list;
}

obj *quote(const obj *args, obj **envp, GarbageCollector *gc) {
  (void) envp;
  (void) gc;
  if (!CHECK_NARGS(args, 1)) return NULL;
  return CAR(args);
}

obj *atom(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* result = eval(CAR(args), envp, gc);
  if (LIST(result)) return is_empty(result) ? t(gc) : empty(gc);
  return ATOM(result) || is_number(result) ? t(gc) : empty(gc);
}

obj *eq(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  //

  obj* first = ith_arg_value(args, envp, 0, gc);
  if (!first) return NULL;
  obj* second = ith_arg_value(args, envp, 1, gc);
  if (!second) return NULL;

  bool same = compare(first, second);
  return same ? t(gc) : empty(gc);
}

obj *car(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), envp, gc);
  if (!LIST(arg_value)) return LOG_ERROR("Argument is not a list");
  return CAR(arg_value);
}

obj *cdr(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), envp, gc);
  if (!LIST(arg_value)) return LOG_ERROR("Argument is not a list");
  return CDR(arg_value);
}

obj *cons(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* x = CAR(args);
  obj* y = ith(args, 1);

  obj* new_cdr = eval(y, envp, gc);
  if (!LIST(new_cdr)) // no dot notation (yet) means second arg must be list
    return LOG_ERROR("Second argument is not list.");

  obj* new_obj = new_list();
  if (new_obj == NULL) return NULL;
  gc_add(gc, new_obj); // Record allocation
  CAR(new_obj) = eval(x, envp, gc);
  CDR(new_obj) = new_cdr;

  return new_obj;
}

obj *cond(const obj *o, obj **envp, GarbageCollector *gc) {
  if (o == NULL) return empty(gc);

  if (!LIST(o)) return LOG_ERROR("Arguments are not a list of pairs");

  obj* pair = CAR(o);
  if (!LIST(pair)) return LOG_ERROR("Conditional pair clause is not a list");
  if (is_empty(pair)) return LOG_ERROR("Empty Conditional pair.");
  if (list_length(pair) != 2)
    return LOG_ERROR("Conditional pair length was %d, not 2.", list_length(pair));

  obj* predicate = eval(CAR(pair), envp, gc);
  if (is_t(predicate)) {
    obj* e = ith(pair, 1);
    if (!e) return LOG_ERROR("Predicate has no associated value");
    return eval(e, envp, gc);
  } else {
    if (!CDR(pair)) return empty(gc); // nothing evaluated to true
    return cond(CDR(o), envp, gc); // recurse on the rest of the list
  }
}

obj *set(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* var_name = ith_arg_value(args, envp, 0, gc);
  if (is_empty(var_name)) return LOG_ERROR("Cannot set empty list");
  if (is_t(var_name)) return LOG_ERROR("Cannot set truth atom");
  if (!ATOM(var_name)) return LOG_ERROR("Can only set atom types");
  obj* value = ith_arg_value(args, envp, 1, gc);

  obj** prev_value_p = lookup_entry(var_name, *envp); // previously bound value
  if (prev_value_p) {
    dispose_recursive(*prev_value_p);  // dispose of the old value
    *prev_value_p = copy_recursive(value); // copy the newly set value into place

  } else { // no previous value
    obj* pair_second = new_list_set(copy_recursive(value), NULL);
    obj* pair_first = new_list_set(copy_recursive(var_name), pair_second);
    obj* new_link = new_list_set(pair_first, *envp);
    *envp = new_link;
  }
  return value;
}

obj *env_prim(const obj *args, obj **envp, GarbageCollector *gc) {
  (void) args; // to avoid "unused argument" warnings
  (void) envp;
  (void) gc;
  if (!check_nargs(__func__, args, 0)) return NULL;
  return *envp;
}

obj *defmacro(const obj *args, obj **envp, GarbageCollector *gc) {
  (void) args;
  (void) envp;
  (void) gc;
  return LOG_ERROR("Macros not yet supported");
}

/**
 * Function: ith_arg_value
 * -----------------------
 * Gets the evaluation of the argument at the nth index of the argument list
 * @param args: The list to get the i'th evaluation of
 * @param envp: The environment to do the evaluation in
 * @param i: The index (starting at 0) of the element to evaluate in o
 * @return: The evaluation of the i'th element of o in the given environment
 */
static obj *ith_arg_value(const obj *args, obj **envp, int i, GarbageCollector *gc) {
  return eval(ith(args, i), envp, gc);
}
