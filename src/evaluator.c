/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include <list.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>
#include <string.h>
#include <lisp-objects.h>
#include <closure.h>
#include <garbage-collector.h>

#define LAMBDA_RESV "lambda"

// Static function declarations
static obj *bind_args_and_captured(const obj *operator, const obj *args, obj **envp, GarbageCollector *gc);
static obj *bind(obj *params, const obj *args, obj **envp, GarbageCollector *gc);
static bool is_lambda(const obj* o);

obj *eval(const obj *o, obj **envp, GarbageCollector *gc) {
  if (!o) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (ATOM(o)) {
    if (is_t(o)) return (obj*) o;
    obj* value = lookup(o, *envp);
    if (value) return value;
    return LOG_ERROR("Variable: \"%s\" not found in environment", ATOM(o));
  }

  // Numbers, primitives and closures evaluate to themselves
  if (is_number(o) || PRIMITIVE(o) || CLOSURE(o)) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (LIST(o)) {
    if (is_lambda(o)) return make_closure(o, *envp, gc);  // Lambda function's value is itself
    if (is_empty(o)) return (obj*) o;                 // Empty list evaluates to itself

    obj* operator = eval(CAR(o), envp, gc);
    return apply(operator, CDR(o), envp, gc);
  }
  return LOG_ERROR("Object of unknown type");
}

obj *apply(const obj *operator, const obj *args, obj **envp, GarbageCollector *gc) {
  if (!operator) return NULL;

  if (PRIMITIVE(operator)) {
    primitive_t f = *PRIMITIVE(operator);
    return f(args, envp, gc);
  }

  if (CLOSURE(operator)) {
    if (!CHECK_NARGS_MAX(args, CLOSURE(operator)->nargs)) return NULL;

    // Partial closure application
    if (list_length(args) < CLOSURE(operator)->nargs)
      return closure_partial_application(operator, args, envp, gc);

    // The result of a closure application is the evaluation of the body of the closure in an environment
    // containing the captured variables from the closure, along with the values of the arguments
    // bound to the parameters of the closure.

    obj* new_env = bind_args_and_captured(operator, args, envp, gc); // Append bound args, and captured vars
    obj* old_env = *envp; // gotta keep one around in case points is modified in eval
    obj* result = eval(PROCEDURE(operator), &new_env, gc); // Evaluate body in prepended environment

    bool split = split_lists(new_env, old_env);
    if (split) gc_add_recursive(gc, new_env); // Mark the bound elements for cleanup

    return result;
  }

  if (ATOM(operator)) return LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(operator));
  return LOG_ERROR("Non-procedure cannot be applied");
}

/**
 * Function: bind_args_and_captured
 * --------------------------------
 * Prepends a list of arguments bound to parameters, and captured variables to an environment
 * @param operator: The operator (closure object) containing the parameter names + captured variables
 * @param args: The arguments to bind to the operator's parameters
 * @param envp: Pointer to the environment to prepend the bound arguments to
 * @return: The new environment
 */
static obj *bind_args_and_captured(const obj *operator, const obj *args, obj **envp, GarbageCollector *gc) {
  obj* new_env = bind(PARAMETERS(operator), args, envp, gc); // Bind the parameters to the arguments
  obj* capture_copy = copy_recursive(CAPTURED(operator));
  return join_lists(capture_copy, new_env); // Prepend the captured list to the environment
}

/**
 * Function: bind
 * --------------
 * binds a list of arguments to parameters and prepends them on to an environment
 * @param params: List of parameters
 * @param args: List of arguments to bind to the parameters
 * @param envp: Environment to prepend the bound arguments to
 * @return: Environment now with bound arguments appended
 */
static obj *bind(obj *params, const obj *args, obj **envp, GarbageCollector *gc) {
  obj* frame = associate(params, args, envp, gc);
  return join_lists(frame, *envp);
}

/**
 * Function: is_lambda
 * -------------------
 * Determines if a list object is a lambda function
 * @param o: The object to determine if it is a lambda
 * @return: True if the object is a lambda function, false otherwise
 */
static bool is_lambda(const obj* o) {
  if (!LIST(o)) return false;
  obj* lambda_atom = CAR(o);
  if (!ATOM(lambda_atom)) return false;
  return strcmp(ATOM(lambda_atom), LAMBDA_RESV) == 0;
}
