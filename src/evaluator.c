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
static obj* bind_args_and_captured(const obj *operator, const obj *args, obj **envp);
static obj* bind(obj *params, const obj *args, obj **envp);
static obj *associate(obj *names, const obj *args, obj **envp);
static bool is_lambda(const obj* o);

obj* eval(const obj* o, obj** envp) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (is_atom(o)) {
    obj* value = lookup(o, *envp);
    if (value) return value;
    return LOG_ERROR("Variable: \"%s\" not found in environment", atom_of(o));
  }

  // Numbers, primitives and closures evaluate to themselves
  if (is_number(o) || is_primitive(o) || is_closure(o)) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (is_list(o)) {
    if (is_lambda(o)) return make_closure(o, *envp);  // Lambda function's value is itself
    if (is_empty(o)) return (obj*) o;                 // Empty list evaluates to itself

    obj* operator = eval(list_of(o)->car, envp);
    obj* args = list_of(o)->cdr;
    return apply(operator, args, envp);
  }
  return LOG_ERROR("Object of unknown type");
}

obj* apply(const obj* operator, const obj* args, obj** envp) {
  if (operator == NULL) return NULL;

  if (is_primitive(operator)) {
    primitive_t f = *primitive_of(operator);
    return f(args, envp);
  }

  if (is_closure(operator)) {
    // The result of a closure application is the evaluation of the body of the closure in an environment
    // containing the captured variables from the closure, along with the values of the arguments
    // bound to the parameters of the closure.

    obj* new_env = bind_args_and_captured(operator, args, envp); // Append bound args, and captured vars
    obj* old_env = *envp; // gotta keep one around in case points is modified in eval
    obj* result =  eval(closure_of(operator)->procedure, &new_env); // Evaluate body in prepended environment

    split_lists(new_env, old_env);
    add_allocated_recursive(new_env); // Mark the bound elements for cleanup

    return result;
  }

  if (is_atom(operator)) return LOG_ERROR("Cannot apply atom: \"%s\" as function", atom_of(operator));
  return LOG_ERROR("Non-procedure cannot be applied");
}

static obj* bind_args_and_captured(const obj *operator, const obj *args, obj **envp) {
  closure_t* closure = closure_of(operator);
  obj* new_env = bind(closure->parameters, args, envp); // Bind the parameters to the arguments
  obj* capture_copy = copy_recursive(closure->captured);
  return join_lists(capture_copy, new_env); // Prepend the captured list to the environment
}

/**
 * Function: bind
 * --------------
 * binds a list of arguments to parameters and prepends them onto an environment
 * @param params: List of parameters
 * @param args: List of arguments to bind to the parameters
 * @param envp: Environment to prepend the bound arguments to
 * @return: Environment now with bound arguments appended
 */
static obj* bind(obj *params, const obj *args, obj **envp) {
  obj* frame = associate(params, args, envp);
  return join_lists(frame, *envp);
}

/**
 * Function: associate
 * -------------------
 * Takes a list of variable names and pairs them up with values in a list of pairs
 * @param names: List of names to associate with values
 * @param args: List of values each associated with the name in the name list
 * @return: A list containing pairs of name-value pairs
 */
static obj *associate(obj *names, const obj *args, obj **envp) {
  if (names == NULL || args == NULL) return NULL;
  if (!is_list(names) || !is_list(args)) return NULL;

  obj* value = eval(list_of(args)->car, envp);
  obj* pair = make_pair(list_of(names)->car,value, true);
  obj* cdr = associate(list_of(names)->cdr, list_of(args)->cdr, envp);
  return new_list_set(pair, cdr);
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
