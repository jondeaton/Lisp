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

#define LAMBDA_RESV "lambda"

// Static function declarations
static obj* put_into_list(obj *o);
static obj* bind(obj* params, obj* args, obj* env);
static obj* associate(obj* names, obj* values);
static obj* push_frame(obj* frame, obj* env);
static obj* eval_list(const obj* list, obj** envp);
static bool is_lambda(const obj* o);

obj* eval(const obj* o, obj** envp) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (o->objtype == atom_obj) {
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
    if (is_empty(o)) return (obj*) o;                 // Empty list evals to itself

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
    obj* arg_values = eval_list(args, envp);

    closure_t* closure = closure_of(operator);
    obj* new_env = bind(closure->parameters, arg_values, *envp);
    return eval(closure->procedure, &new_env);
  }

  if (is_atom(operator)) return LOG_ERROR("Cannot apply atom: \"%s\" as function", atom_of(operator));
  return LOG_ERROR("Application of non-procedure");
}

/**
 * Function: bind
 * --------------
 * binds a list of arguments to parameters and pushes them onto an environment
 * @param params: List of parameters
 * @param args: List of arguments to bind to the parameters
 * @param env: Environment to prepend the bound arguments to
 * @return: Environment now with bound arguments appended
 */
static obj* bind(obj* params, obj* args, obj* env) {
  obj* frame = associate(params, args);
  return push_frame(frame, env);
}

/**
 * Function: eval_list
 * -------------------
 * Evaluate a list, creating a new list with the evaluated arguments
 * @param list: A lisp object that is a list to evaluate each element of
 * @param envp: Environment to evaluate the elements of the list
 * @return: A new list with the evaluated values of the passed list
 */
static obj* eval_list(const obj* list, obj** envp) {
  if (list == NULL || !is_list(list)) return NULL;
  obj* o = put_into_list(eval(list_of(list)->car, envp));
  list_of(o)->cdr = eval_list(list_of(list)->cdr, envp);
  return o;
}

/**
 * Function: associate
 * -------------------
 * Takes a list of variable names and pairs them up with values in a list of pairs
 * @param names: List of names to associate with values
 * @param values: List of values each associated with the name in the name list
 * @return: A list containing pairs of name-value pairs
 */
static obj* associate(obj* names, obj* values) {
  if (names == NULL || values == NULL) return NULL;
  if (!is_list(names) || !is_list(values)) return NULL;

  obj* pair = make_pair(list_of(names)->car, list_of(values)->car);
  obj* cdr = associate(list_of(names)->cdr, list_of(values)->cdr);
  return new_list_set(pair, cdr);
}

/**
 * Function: push_frame
 * --------------------
 * Pushes a list of pairs of associated keys and values onto a list of such pairs
 * @param frame: List of name-value pairs
 * @param env: List of name-value pairs to append the frame to
 * @return: The new augmented list of pairs, with the frame on the front
 */
static obj* push_frame(obj* frame, obj* env) {
  if (frame == NULL) return env;
  if (env == NULL) return frame;
  if (!is_list(frame) || !is_list(env)) return NULL;

  if (list_of(frame)->cdr == NULL) list_of(frame)->cdr = env;
  else push_frame(list_of(frame)->cdr, env);

  return frame;
}

/**
 * Function: put_into_list
 * -----------------------
 * Makes a list object with car pointing to the object passed
 * @param o: The object that the list's car should point to
 * @return: A pointer to the list object containing only the argument object
 */
static obj* put_into_list(obj *o) {
  return new_list_set(o, NULL);
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
