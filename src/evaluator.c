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
#include <memory-manager.h>

#define LAMBDA_RESV "lambda"

// Static function declarations
static obj *bind_args_and_captured(const obj *operator, const obj *args, obj **envp, MemoryManager *gc);
static obj *bind(obj *params, const obj *args, obj **envp, MemoryManager *gc);

obj *eval(const obj *o, obj **envp, MemoryManager *gc) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (is_atom(o)) {
    if (is_t(o)) return (obj*) o;
    obj* value = lookup(o, *envp);
    if (value == NULL) {
      LOG_ERROR("Variable: \"%s\" not found in environment", ATOM(o));
      return NULL;
    }
    return value;
  }

  // Numbers, primitives and closures evaluate to themselves
  if (is_number(o) || is_primitive(o) || is_closure(o)) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (is_list(o)) {
    if (is_nil(o)) return (obj*) o;                     // Empty list evaluates to itself

    obj* oper = eval(CAR(o), envp, gc);
    return apply(oper, CDR(o), envp, gc);
  }
  LOG_ERROR("Object of unknown type");
  return NULL;
}

obj *apply(const obj *oper, const obj *args, obj **envp, MemoryManager *gc) {
  if (oper == NULL) return NULL;

  if (is_primitive(oper)) {
    primitive_t f = *PRIMITIVE(oper);
    return f(args, envp, gc);
  }

  if (is_closure(oper)) {
    if (!CHECK_NARGS_MAX(args, NARGS(oper))) return NULL;

    // Partial closure application
    if (list_length(args) < NARGS(oper))
      return closure_partial_application(oper, args, envp, gc);

    // The result of a closure application is the evaluation of the body of the closure in an environment
    // containing the captured variables from the closure, along with the values of the arguments
    // bound to the parameters of the closure.

    obj* new_env = bind_args_and_captured(oper, args, envp, gc); // Append bound args, and captured vars
    obj* old_env = *envp; // gotta keep one around in case points is modified in eval
    obj* result = eval(PROCEDURE(oper), &new_env, gc); // Evaluate body in prepended environment

    bool split = split_lists(new_env, old_env);
    if (split) mm_add_recursive(gc, new_env); // Mark the bound elements for cleanup

    return result;
  }

  if (is_atom(oper)) {
    LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(oper));
    return NULL;
  }
  LOG_ERROR("Non-procedure cannot be applied");
  return NULL;
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
static obj *bind_args_and_captured(const obj *operator, const obj *args, obj **envp, MemoryManager *gc) {
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
static obj *bind(obj *params, const obj *args, obj **envp, MemoryManager *gc) {
  obj* frame = associate(params, args, envp, gc);
  return join_lists(frame, *envp);
}
