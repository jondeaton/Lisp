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
static def_primitive(defmacro_form);
static def_primitive(lisp_list);
static def_primitive(env);
static def_primitive(lambda);

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "defmacro",
                                             "list", "env", "lambda", NULL };

static const primitive_t primitive_functions[] = { &quote, &atom, &eq, &car, &cdr, &cons,
                                                   &cond, &set, &defmacro_form,
                                                   &lisp_list, &env, &lambda, NULL };

// Helper: after binding a closure, update CAPTURED so it can reference itself (for recursion)
static void enable_self_recursion(obj *value, obj *env_before, obj *env_after) {
  if ((is_closure(value) || is_macro(value)) && CAPTURED(value) == env_before)
    CAPTURED(value) = env_after;
}

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

obj *t(GarbageCollector *gc) { return gc->t_cached; }
obj *nil(GarbageCollector *gc) { return gc->nil_cached; }

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
  CDR(new_obj) = is_nil(cdr) ? NULL : cdr;

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

  obj* env_before = interpreter->env;
  obj** prev_value_p = lookup_entry(var_name, interpreter->env);
  if (prev_value_p == NULL) {
    obj* pair = make_pair(var_name, value);
    obj* new_link = new_list_set(pair, interpreter->env);
    gc_add(&interpreter->gc, CDR(pair));
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, new_link);
    interpreter->env = new_link;
  } else {
    *prev_value_p = value;
  }
  enable_self_recursion(value, env_before, interpreter->env);
  return value;
}

static def_primitive(defmacro_form) {
  if (!CHECK_NARGS(args, 3)) return NULL;

  obj* name = CAR(args);
  if (!is_atom(name) || is_t(name)) {
    LOG_ERROR("defmacro: first argument must be a name");
    return NULL;
  }

  obj* params = ith(args, 1);
  if (!is_list(params)) {
    LOG_ERROR("defmacro: parameters must be a list");
    return NULL;
  }

  obj* body = ith(args, 2);

  obj* o = new_closure_set(params, body, interpreter->env);
  if (o == NULL) return NULL;
  o->objtype = macro_obj;
  gc_add(&interpreter->gc, o);

  obj** prev_value_p = lookup_entry(name, interpreter->env);
  if (prev_value_p == NULL) {
    obj* pair = make_pair(name, o);
    obj* new_link = new_list_set(pair, interpreter->env);
    gc_add(&interpreter->gc, CDR(pair));
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, new_link);
    interpreter->env = new_link;
  } else {
    *prev_value_p = o;
  }
  CAPTURED(o) = interpreter->env;
  return o;
}

static def_primitive(lisp_list) {
  if (args == NULL || is_nil(args)) return nil(&interpreter->gc);

  obj* first = eval(CAR(args), interpreter);
  if (first == NULL) {
    LOG_ERROR("Error evaluating list element");
    return NULL;
  }

  obj* rest = NULL;
  if (CDR(args) != NULL) {
    rest = lisp_list(CDR(args), interpreter);
    if (rest == NULL) return NULL;
    if (is_nil(rest)) rest = NULL;
  }

  obj* result = new_list_set(first, rest);
  gc_add(&interpreter->gc, result);
  return result;
}

static def_primitive(env) {
  if (!check_nargs(__func__, args, 0)) return NULL;
  return interpreter->env;
}

/**
 * Primitive: lambda
 * -----------------
 * Define a non-primitive procedure. Captures the entire lexical environment.
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
  obj* o = new_closure_set(params, procedure, interpreter->env);
  if (o == NULL) {
    LOG_ERROR("Error allocating closure object");
    return NULL;
  }

  gc_add(&interpreter->gc, o);
  return o;
}
