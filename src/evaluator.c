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
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (is_atom(o)) {
    if (is_t(o)) return (obj*) o;
    obj* value = lookup(o, *envp);
    if (value) return value;
    return LOG_ERROR("Variable: \"%s\" not found in environment", atom_of(o));
  }

  // Numbers, primitives and closures evaluate to themselves
  if (is_number(o) || is_primitive(o) || is_closure(o)) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (is_list(o)) {
    if (is_lambda(o)) return make_closure(o, *envp, gc);  // Lambda function's value is itself
    if (is_empty(o)) return (obj*) o;                 // Empty list evaluates to itself

    obj* operator = eval(list_of(o)->car, envp, gc);
    obj* args = list_of(o)->cdr;
    return apply(operator, args, envp, gc);
  }
  return LOG_ERROR("Object of unknown type");
}

obj *apply(const obj *operator, const obj *args, obj **envp, GarbageCollector *gc) {
  if (operator == NULL) return NULL;

  if (is_primitive(operator)) {
    primitive_t f = *primitive_of(operator);
    return f(args, envp, gc);
  }

  if (is_closure(operator)) {
    if (!CHECK_NARGS_MAX(args, closure_of(operator)->nargs)) return NULL;

    // Partial closure application
    if (list_length(args) < closure_of(operator)->nargs)
      return closure_partial_application(operator, args, envp, gc);

    // The result of a closure application is the evaluation of the body of the closure in an environment
    // containing the captured variables from the closure, along with the values of the arguments
    // bound to the parameters of the closure.

    obj* new_env = bind_args_and_captured(operator, args, envp, gc); // Append bound args, and captured vars
    obj* old_env = *envp; // gotta keep one around in case points is modified in eval
    obj* result = eval(closure_of(operator)->procedure, &new_env, gc); // Evaluate body in prepended environment

    bool split = split_lists(new_env, old_env);
    if (split) gc_add_recursive(gc, new_env); // Mark the bound elements for cleanup

    return result;
  }

  if (is_atom(operator)) return LOG_ERROR("Cannot apply atom: \"%s\" as function", atom_of(operator));
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
  closure_t* closure = closure_of(operator);
  obj* new_env = bind(closure->parameters, args, envp, gc); // Bind the parameters to the arguments
  obj* capture_copy = copy_recursive(closure->captured);
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
  if (o == NULL) return false;
  if (o->objtype != list_obj) return false;
  obj* lambda_atom = list_of(o)->car;
  if (lambda_atom == NULL) return false;
  if (lambda_atom->objtype != atom_obj) return false;
  return strcmp(atom_of(lambda_atom), LAMBDA_RESV) == 0;
}
