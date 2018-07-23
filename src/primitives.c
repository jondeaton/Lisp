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
static bool capture_variables(obj **capturedp, const obj *params, const obj *procedure, const obj *env);


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
  if (value == NULL) {
    LOG_ERROR("Error evaluating right-hand-side");
    return NULL;
  }

  // Make a copy of the result
  obj* result_cpy = copy_recursive(value);
  if (result_cpy == NULL) {
    LOG_ERROR("Error copying right-hand-side");
    return NULL;
  }

  // Store the result in the environment (potentially over-writing)
  obj** prev_value_p = lookup_entry(var_name, *envp); // previously bound value
  if (prev_value_p == NULL) {
    // no previous value found in environment
    obj* pair_second = new_list_set(result_cpy, NULL);
    obj *var_name_copy = copy_recursive(var_name);
    obj *pair_first = new_list_set(var_name_copy, pair_second);
    obj *new_link = new_list_set(pair_first, *envp);

    if (var_name_copy == NULL || pair_first == NULL ||
        pair_second == NULL || new_link == NULL) {
      LOG_ERROR("Error allocating memory to store variable in environment");
      dispose_recursive(result_cpy);
      dispose_recursive(var_name_copy);
      dispose_recursive(pair_first);
      dispose_recursive(new_link);
      return NULL;
    }

    *envp = new_link;
  } else {
    // Over-write previous value
    // Note: must copy the result into a temporary value *before* disposing of the
    // previous value because the result value may reference the previous value, as in the
    // case of self-referential over-writing. For example: (set 'x (cdr x))
    dispose_recursive(*prev_value_p);         // dispose of the old value
    *prev_value_p = result_cpy;               // store new value in environment
    value = result_cpy;
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

  if (params ==  NULL || procedure == NULL) {
    LOG_ERROR("Error copying parameters and body of lambda declaration");
    dispose_recursive(params);
    dispose_recursive(procedure);
    return NULL;
  }

  // Capture variables
  obj* captured = NULL; // Will store the captured variables
  bool success = capture_variables(&captured, params, procedure, *envp);
  if (!success) {
    LOG_ERROR("Error while capturing lambda variables");
    dispose_recursive(params);
    dispose_recursive(procedure);
    return NULL;
  }

  // Create new closure object
  obj* o = new_closure_set(params, procedure, captured);
  if (o == NULL) {
    LOG_ERROR("Error allocating closure object");
    dispose_recursive(params);
    dispose_recursive(procedure);
    return NULL;
  }

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
 * Function: capture_variables
 * ---------------------------
 * Creates a captured variable list by searching for variable names that exist in both the procedure
 * and the environment. Creates a list of key-value pairs extracted (copied) from the environment.
 * Variable names in the parameter list will not be captured.
 * @param capturedp: Pointer to where the captured list reference should be stored
 * @param params: Parameters to the lambda function (these will not be captured
 * @param procedure: Procedure body of the lambda function to search for variables to bind in
 * @param env: Environment to search for values to capture
 * @return true if variables were captures successfully, valse otherwise
 */
static bool capture_variables(obj **capturedp, const obj *params,
                              const obj *procedure, const obj *env) {
  if (procedure == NULL) return true;

  if (is_atom(procedure)) {
    if (lookup_pair(procedure, *capturedp)) return true; // Already captured
    // Don't capture parameters (those get bound at apply-time)
    if (list_contains(params, procedure)) return true;

    obj* matching_pair = lookup_pair(procedure, env);
    if (matching_pair == NULL) return true; // No value to be captured

    obj *pair_copy = copy_recursive(matching_pair);
    if (pair_copy == NULL)
      return false;
    obj *new_list = new_list_set(pair_copy, *capturedp); // Prepend to capture list
    if (new_list == NULL) {
      dispose_recursive(pair_copy);
      return false;
    }
    *capturedp = new_list;

  } else if (is_list(procedure)) { // depth-first search
    bool success = capture_variables(capturedp, params, CAR(procedure), env);
    if (!success)
      return false;
    return capture_variables(capturedp, params, CDR(procedure), env); // tail recursion
  }
  return true;
}
