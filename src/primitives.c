/*
 * File: primitives.c
 * ------------------
 * Presents the implementation of the lisp primitives.
 */

#include <interpreter.h>
#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <list.h>
#include <closure.h>

#include <assert.h>
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
static def_primitive(lambda);

const obj lisp_NIL;
const obj lisp_T;

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "env", "lambda", NULL };

static const primitive_t primitive_functions[] = { &quote, &atom, &eq, &car, &cdr, &cons,
                                                   &cond, &set, &env, &lambda, NULL };

// Static function declarations
static bool capture_variables(obj **capturedp, const obj *params, const obj *procedure, const obj *env);

obj* get_primitive_library(void) {
  return create_environment(primitive_reserved_names, primitive_functions);
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = primitive_obj;
  o->reachable = false;
  o->primitive = primitive;
  return o;
}

obj *t(GarbageCollector *gc) {
  obj* t = new_atom("t");
  gc_add(gc, t);
  return t;
}

obj *nil(GarbageCollector *gc) {
  obj* list = new_list_set(NULL, NULL);
  gc_add(gc, list);
  return list;
}

static def_primitive(quote) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  return CAR(args);
}

static def_primitive(atom) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* result = eval(CAR(args), interpreter);
  if (is_list(result)) return is_nil(result) ? t(&interpreter->gc) : nil(&interpreter->gc);
  if (is_atom(result)) return t(&interpreter->gc);
  return is_number(result) ? t(&interpreter->gc) : nil(&interpreter->gc);
}

static def_primitive(eq) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), interpreter);
  if (first == NULL) return NULL;
  obj* second = eval(ith(args, 1), interpreter);
  if (second == NULL) return NULL;

  bool same = compare(first, second);
  return same ? t(&interpreter->gc) : nil(&interpreter->gc);
}

static def_primitive(car) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), interpreter);
  if (arg_value == NULL) {
    LOG_ERROR("Error evaluating argument");
    return NULL;
  }

  if (!is_list(arg_value)) {
    LOG_ERROR("Argument is not a list");
    return NULL;
  }
  if (is_nil(arg_value)) return nil(&interpreter->gc);
  return CAR(arg_value);
}

static def_primitive(cdr) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), interpreter);
  if (arg_value == NULL) {
    LOG_ERROR("Error evaluating argument");
    return NULL;
  }

  if (!is_list(arg_value)) {
    LOG_ERROR("Argument is not a list");
    return NULL;
  }

  if (is_nil(arg_value)) return nil(&interpreter->gc);
  if (CDR(arg_value) == NULL) return nil(&interpreter->gc);
  return CDR(arg_value);
}

static def_primitive(cons) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* y = ith(args, 1);
  if (y == NULL) {
    LOG_ERROR("Could not get second argument");
    return NULL;
  }

  obj *car = eval(CAR(args), interpreter);
  if (car == NULL) {
    LOG_ERROR("Error evaluating first argument");
    return NULL;
  }

  obj* cdr = eval(y, interpreter);
  if (cdr == NULL) {
    LOG_ERROR("Error evaluating second argument");
    return NULL;
  }

  if (!is_list(cdr)) {
    LOG_ERROR("Second argument is not list");
    return NULL;
  }

  obj *new_obj = new_list();
  if (new_obj == NULL) {
    LOG_ERROR("could not allocate list element");
    return NULL;
  }
  gc_add(&interpreter->gc, new_obj);

  CAR(new_obj) = car;
  CDR(new_obj) = cdr;

  return new_obj;
}

static def_primitive(cond) {
  if (args == NULL) return nil(&interpreter->gc);

  if (!is_list(args)) {
    LOG_ERROR("Arguments are not a list of pairs");
    return NULL;
  }

  obj *pair = CAR(args);

  if (!is_list(pair)) {
    LOG_ERROR("Conditional pair clause is not a list");
    return NULL;
  }

  if (is_nil(pair)) {
    LOG_ERROR("Empty Conditional pair.");
    return NULL;
  }

  if (list_length(pair) != 2) {
    LOG_ERROR("Conditional pair length was %d, not 2.", list_length(pair));
    return NULL;
  }

  obj *predicate = eval(CAR(pair), interpreter);
  if (is_primitive(predicate)) {
    LOG_ERROR("Cannot cast primitive function as bool.");
    return NULL;
  }

  if (!is_nil(predicate)) {
    obj* e = ith(pair, 1);
    if (e == NULL) {
      LOG_ERROR("Predicate has no associated value");
      return NULL;
    }
    obj *value = eval(e, interpreter);
    if (value == NULL)
      LOG_ERROR("Error evaluating value for predicate");
    return value;
  }

  return cond(CDR(args), interpreter);
}

/**
 * Primitive: set
 * --------------
 * Bind a value to a name in the environment.
 * Usage: (set 'foo 42)
 */
static def_primitive(set) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* var_name = eval(ith(args, 0), interpreter);
  if (is_nil(var_name)) {
    LOG_ERROR("Cannot set empty list");
    return NULL;
  }
  if (is_t(var_name)) {
    LOG_ERROR("Cannot set truth atom");
    return NULL;
  }
  if (!is_atom(var_name)) {
    LOG_ERROR("Can only set atom types");
    return NULL;
  }
  obj* value = eval(ith(args, 1), interpreter);
  if (value == NULL) {
    LOG_ERROR("Error evaluating right-hand-side");
    return NULL;
  }

  // Check if already bound
  obj** prev_value_p = lookup_entry(var_name, interpreter->env);
  if (prev_value_p == NULL) {
    // New binding: prepend (name value) pair to env
    obj* pair = make_pair(var_name, value);
    obj* new_link = new_list_set(pair, interpreter->env);
    gc_add(&interpreter->gc, CDR(pair));
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, new_link);
    interpreter->env = new_link;
  } else {
    // Overwrite previous value (GC will collect old value if unreachable)
    *prev_value_p = value;
  }
  return value;
}

static def_primitive(env) {
  if (!check_nargs(__func__, args, 0)) return NULL;
  return interpreter->env;
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
  if (!is_list(params)) {
    LOG_ERROR("Lambda parameters are not a list");
    return NULL;
  }

  FOR_LIST(params, var) {
    if (var == NULL) continue;
    if (is_t(var)) {
      LOG_ERROR("Truth atom can't be parameter");
      return NULL;
    }
    if (is_nil(var)) {
      LOG_ERROR("Empty list can't be a parameter");
      return NULL;
    }
    if (!is_atom(var)) {
      LOG_ERROR("Parameter was not an atom");
      return NULL;
    }
  }

  obj* procedure = ith(args, 1);

  // Capture variables
  obj* captured = NULL;
  bool success = capture_variables(&captured, params, procedure, interpreter->env);
  if (!success) {
    LOG_ERROR("Error while capturing lambda variables");
    return NULL;
  }

  obj* o = new_closure_set(params, procedure, captured);
  if (o == NULL) {
    LOG_ERROR("Error allocating closure object");
    return NULL;
  }

  gc_add(&interpreter->gc, o);
  gc_add_recursive(&interpreter->gc, captured);
  return o;
}

/**
 * Function: capture_variables
 * ---------------------------
 * Creates a captured variable list by searching for variable names that exist in both the procedure
 * and the environment. Captured values are copied so closures have snapshot semantics.
 */
static bool capture_variables(obj **capturedp, const obj *params,
                              const obj *procedure, const obj *env) {
  if (procedure == NULL) return true;

  if (is_atom(procedure)) {
    if (lookup_pair(procedure, *capturedp)) return true; // Already captured
    if (list_contains(params, procedure)) return true;   // Parameter, not free var

    obj* matching_pair = lookup_pair(procedure, env);
    if (matching_pair == NULL) return true; // No value to capture

    obj *pair_copy = copy_recursive(matching_pair);
    if (pair_copy == NULL) return false;

    obj *new_list = new_list_set(pair_copy, *capturedp);
    if (new_list == NULL) return false;
    *capturedp = new_list;

  } else if (is_list(procedure)) {
    bool success = capture_variables(capturedp, params, CAR(procedure), env);
    if (!success) return false;
    return capture_variables(capturedp, params, CDR(procedure), env);
  }
  return true;
}
