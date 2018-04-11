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

// forward declarations of primitives
static def_primitive(quote);
static def_primitive(atom);
static def_primitive(eq);
static def_primitive(car);
static def_primitive(cdr);
static def_primitive(cons);
static def_primitive(cond);
static def_primitive(set);
static def_primitive(env);
static def_primitive(defmacro);

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "env", "defmacro", NULL };

static const primitive_t primitive_functions[] = {&quote, &atom, &eq, &car, &cdr, &cons,
                                                  &cond, &set, &env, &defmacro,  NULL };

// Static function declarations
static obj *ith_arg_value(const obj *args, obj **envp, int i, GarbageCollector *gc);

obj* get_primitive_library() {
  return create_environment(primitive_reserved_names, primitive_functions);
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  MALLOC_CHECK(o);
  o->objtype = primitive_obj;
  memcpy(PRIMITIVE(o), &primitive, sizeof(primitive));
  return o;
}

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

/**
 * Primitive: quote
 * ----------------
 * Returns the unevaluated version of the object
 * @param args: The object to quote
 * @param envp: The environment to evaluate this primitive in
 * @return: Pointer to the lisp object without evaluating it
 */
static def_primitive(quote) {
  (void) envp;
  (void) gc;
  if (!CHECK_NARGS(args, 1)) return NULL;
  return CAR(args);
}

/**
 * Primitive: atom
 * ---------------
 * Checks if an object is an atom
 */
static def_primitive(atom) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* result = eval(CAR(args), envp, gc);
  if (is_list(result)) return is_empty(result) ? t(gc) : empty(gc);
  if (is_atom(result)) return t(gc);
  return is_number(result) ? t(gc) : empty(gc);
}

/**
 * Primitive: eq
 * -------------
 * Test for equality of two objects
 */
static def_primitive(eq) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = ith_arg_value(args, envp, 0, gc);
  if (!first) return NULL;
  obj* second = ith_arg_value(args, envp, 1, gc);
  if (!second) return NULL;

  bool same = compare(first, second);
  return same ? t(gc) : empty(gc);
}

/**
 * Primitive: car
 * -------------
 * Expects the value of l to be a list and returns it's first element
 */
static def_primitive(car) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), envp, gc);
  if (!is_list(arg_value)) return LOG_ERROR("Argument is not a list");
  return CAR(arg_value);
}

/**
 * Primitive: cdr
 * -------------
 * Expects the value of l to be a list and returns everything after the first element
 */
static def_primitive(cdr) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), envp, gc);
  if (!is_list(arg_value)) return LOG_ERROR("Argument is not a list");
  return CDR(arg_value);
}

/**
 * Primitive: cons
 * ---------------
 * Expects the value of y to be a list and returns a list containing the value
 * of x followed by the elements of the value of y
 */
static def_primitive(cons) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* y = ith(args, 1);

  obj* new_cdr = eval(y, envp, gc);
  if (!is_list(new_cdr)) // no dot notation (yet) means second arg must be list
    return LOG_ERROR("Second argument is not list.");

  obj* new_obj = new_list();
  if (new_obj == NULL) return NULL;
  gc_add(gc, new_obj); // Record allocation
  CAR(new_obj) = eval(CAR(args), envp, gc);
  CDR(new_obj) = new_cdr;

  return new_obj;
}

/**
 * Primitive: cond
 * ---------------
 * (cond (p1 e1) ... (pn en))
 * The p expressions are evaluated in order until one returns t
 * When one is found  the value of the corresponding e expression
 * is returned as the expression
 */
static def_primitive(cond) {
  if (args == NULL) return empty(gc);

  if (!is_list(args)) return LOG_ERROR("Arguments are not a list of pairs");

  obj* pair = CAR(args);
  if (!is_list(pair)) return LOG_ERROR("Conditional pair clause is not a list");
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
    return cond(CDR(args), envp, gc); // recurse on the rest of the list
  }
}

/**
 * Primitive: set
 * --------------
 * Primitive function for setting a value in the environment that the
 * primitive is evaluated in
 * Usage: (set 'foo 42)
 */
static def_primitive(set) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* var_name = ith_arg_value(args, envp, 0, gc);
  if (is_empty(var_name)) return LOG_ERROR("Cannot set empty list");
  if (is_t(var_name)) return LOG_ERROR("Cannot set truth atom");
  if (!is_atom(var_name)) return LOG_ERROR("Can only set atom types");
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

/**
 * Primitive: env
 * --------------
 * Simply returns the environment
 */
static def_primitive(env) {
  (void) args; // to avoid "unused argument" warnings
  (void) envp;
  (void) gc;
  if (!check_nargs(__func__, args, 0)) return NULL;
  return *envp;
}

/**
 * Primitive: defmacro
 * -------------------
 * Defines a lisp macro
 */
static def_primitive(defmacro) {
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
