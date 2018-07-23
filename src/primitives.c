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
#include <closure.h>

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
static def_primitive(lambda);
static def_primitive(defmacro);

const obj lisp_NIL; // The empty list / NIL value within lisp.
const obj lisp_T;   // The true atom.

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "env", "lambda", "defmacro", NULL };

static const primitive_t primitive_functions[] = {&quote, &atom, &eq, &car, &cdr, &cons,
                                                  &cond, &set, &env, &lambda, &defmacro,  NULL };

// Static function declarations
static obj *ith_arg_value(const obj *args, obj **envp, int i, GarbageCollector *gc);
static void get_captured_vars(obj **capturedp, const obj *params, const obj *procedure, const obj *env);


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
obj *nil(GarbageCollector *gc) {
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
  if (is_list(result)) return is_nil(result) ? t(gc) : nil(gc);
  if (is_atom(result)) return t(gc);
  return is_number(result) ? t(gc) : nil(gc);
}

/**
 * Primitive: eq
 * -------------
 * Test for equality of two objects
 */
static def_primitive(eq) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = ith_arg_value(args, envp, 0, gc);
  if (first == NULL) return NULL;
  obj* second = ith_arg_value(args, envp, 1, gc);
  if (second == NULL) return NULL;

  bool same = compare(first, second);
  return same ? t(gc) : nil(gc);
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
  if (is_nil(arg_value)) return nil(gc);
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
  if (is_nil(arg_value)) return nil(gc);
  if (CDR(arg_value) == NULL) return nil(gc);
  return CDR(arg_value);
}

/**
 * Primitive: cons
 * ---------------
 * Usage (cons 'x '(a b c)) --> (x a b c)
 *
 * (cons x y)
 * Expects the value of y to be a list and returns a list containing the value
 * of x followed by the elements of the value of y
 */
static def_primitive(cons) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* y = ith(args, 1);
  obj* new_cdr = eval(y, envp, gc);
  if (new_cdr == NULL) return NULL; // todo: log error?
  if (!is_list(new_cdr)) // no dot notation (yet) means second arg must be list
    return LOG_ERROR("Second argument is not list.");

  // Allocate new slot to hold x in result list
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
  if (args == NULL) return nil(gc);

  if (!is_list(args)) return LOG_ERROR("Arguments are not a list of pairs");

  obj* pair = CAR(args);
  if (!is_list(pair)) return LOG_ERROR("Conditional pair clause is not a list");
  if (is_nil(pair)) return LOG_ERROR("Empty Conditional pair.");
  if (list_length(pair) != 2)
    return LOG_ERROR("Conditional pair length was %d, not 2.", list_length(pair));

  obj* predicate = eval(CAR(pair), envp, gc);
  if (is_primitive(predicate)) {
    LOG_ERROR("Cannot cast primitive function as bool.");
    return NULL;
  }

  if (is_t(predicate)) {
    // recursive base case: predicate evaluated to true
    obj* e = ith(pair, 1); // get it's associated value
    if (e == NULL) {
      LOG_ERROR("Predicate has no associated value");
      return NULL;
    }
    return eval(e, envp, gc);
  }

  // recursive base case: no predicates evaluated to true
  if (CDR(pair) == NULL) return nil(gc);

  // tail recursion on the remaining predicate-expression pairs
  return cond(CDR(args), envp, gc);
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
  if (is_nil(var_name)) return LOG_ERROR("Cannot set empty list");
  if (is_t(var_name)) return LOG_ERROR("Cannot set truth atom");
  if (!is_atom(var_name)) return LOG_ERROR("Can only set atom types");
  obj* value = ith_arg_value(args, envp, 1, gc);

  obj** prev_value_p = lookup_entry(var_name, *envp); // previously bound value
  if (prev_value_p != NULL) {
    // todo: gotta copy before disposing because what you are disposing of recursively
    // might be part of the new value! Use a temporary pointer to accomplish this

    // Note: You must copy the result into a temporary value *before* disposing of the
    // previous value because the result value may reference the previous value in the
    // case of self-referential over-writing, for example: (set 'x (cdr x))
//    obj* result_cpy = copy_recursive(value); // copy the newly set value into place
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
 * Primitive: lambda
 * -----------------
 * Define a non-primitive procedure
 */
static def_primitive(lambda) {
  if (!CHECK_NARGS_MIN(args, 1)) return NULL;
  if (!CHECK_NARGS_MAX(args, 2)) return NULL;

  obj* params = ith(args, 0);
  if (!is_list(params)) return LOG_ERROR("Lambda parameters are not a list");
  FOR_LIST(params, var) {
    if (var == NULL) continue;
    if (is_t(var))     return LOG_ERROR("Truth atom can't be parameter");
    if (is_nil(var)) return LOG_ERROR("Empty list can't be a parameter");
    if (!is_atom(var)) return LOG_ERROR("Parameter was not an atom");
  }
  params = copy_recursive(params); // Params are well-formed, make a copy for saving.

  obj* procedure = copy_recursive(ith(args, 1));

  obj* captured = NULL; // Will store the captured variables
  get_captured_vars(&captured, args, procedure, *envp);

  obj* o = new_closure_set(params, procedure, captured);
  gc_add_recursive(gc, o);
  return o;
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

/**
 * Function: get_captured_vars
 * ---------------------------
 * Creates a captured variable list by searching for variable names that exist in both the procedure
 * and the environment. Creates a list of key-value pairs extracted (copied) from the environment.
 * Variable names in the parameter list will not be captured.
 * @param capturedp: Pointer to where the captured list reference should be stored
 * @param params: Parameters to the lambda function (these will not be captured
 * @param procedure: Procedure body of the lambda function to search for variables to bind in
 * @param env: Environment to search for values to capture
 */
static void get_captured_vars(obj **capturedp, const obj *params, const obj *procedure, const obj *env) {
  if (procedure == NULL) return;

  if (is_list(procedure)) { // depth-first search
    get_captured_vars(capturedp, params, CAR(procedure), env);
    get_captured_vars(capturedp, params, CDR(procedure), env);

  } else if (is_atom(procedure)) {
    if (lookup_pair(procedure, *capturedp)) return; // Already captured
    if (list_contains(params, procedure)) return; // Don't capture parameters (those get bound at apply-time)

    obj* matching_pair = lookup_pair(procedure, env);
    if (!matching_pair) return; // No value to be captured
    *capturedp = new_list_set(copy_recursive(matching_pair), *capturedp); // Prepend to capture list
  }
}
