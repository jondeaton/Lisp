/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include "evaluator.h"
#include "list.h"
#include "environment.h"
#include <stdlib.h>
#include <string.h>
#include <stack-trace.h>
#include <list.h>
#include "stdio.h"

#define LAMBDA_RESV "lambda"

// Vector of allocated objects needing to be free'd
CVector* allocated;

// Static function declarations
static obj* put_into_list(obj *o);
static obj* bind(obj* params, obj* args, obj* env);
static obj* associate(obj* names, obj* values);
static obj* push_frame(obj *frame, obj *env);
static obj* eval_list(const obj* list, obj *env);
static bool is_lambda(const obj* o);
static void obj_cleanup(obj** o_ref);

obj* eval(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (o->objtype == atom_obj) {
    obj* value = lookup(o, env);
    if (value) return value;
    log_error(__func__, sprintf("Atom: %s not found in environment", (char*) atom_of(o)));
    return NULL;
  }

  // Numbers evaluate to themselves
  if (o->objtype == integer_obj || o->objtype == float_obj) return (obj*) o;

  // Primitive means that there's nothing left to apply
  if (o->objtype == primitive_obj) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (o->objtype == list_obj) {
    if (is_lambda(o)) return (obj*) o;  // Lambda function's value is itself
    if (is_empty(o)) return (obj*) o;   // Empty list's value is itself

    obj* operator = eval(list_of(o)->car, env);
    return apply(operator, list_of(o)->cdr, env);
  }
  log_error(__func__, "Object of unknown type");
  return NULL;
}

obj* apply(const obj* operator, const obj* args, obj* env) {
  if (operator == NULL) return NULL;
  if (operator->objtype == atom_obj) {
    log_error(__func__, sprintf("Cannot apply atom: \"%s\" as function", (char*) atom_of(operator)));
    return NULL;
  }

  if (operator->objtype == primitive_obj) {
    primitive_t prim = *primitive_of(operator);
    return prim(args, env);

  } else {
    obj* arg_values = eval_list(args, env);

    if (!is_lambda(operator)) return NULL; // <-- not a lambda function

    obj* params = list_of(list_of(operator)->cdr)->car;
    obj* new_env = bind(params, arg_values, env);
    obj* exp = list_of(list_of(list_of(operator)->cdr)->cdr)->car;
    return eval(exp, new_env);
  }
}

void init_allocated() {
  allocated = cvec_create(sizeof(obj*), 0, (CleanupElemFn) &obj_cleanup);
}

void add_allocated(const obj* o) {
  cvec_append(allocated, &o);
}

void clear_allocated() {
  cvec_clear(allocated);
}

void dispose_allocated() {
  cvec_dispose(allocated);
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
 * @param env: Environment to evaluate the elements of the list
 * @return: A new list with the evaluated values of the passed list
 */
static obj* eval_list(const obj *list, obj *env) {
  if (list == NULL) return NULL;
  if (list->objtype != list_obj) return NULL;
  obj* o = put_into_list(eval(list_of(list)->car, env));
  list_of(o)->cdr = eval_list(list_of(list)->cdr, env);
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
  if (names->objtype != list_obj || values->objtype != list_obj) return NULL;

  obj* pair = make_pair(list_of(names)->car, list_of(values)->car);

  obj* pairs = new_list();
  list_of(pairs)->car = pair;

  list_of(pairs)->cdr = associate(list_of(names)->cdr, list_of(values)->cdr);
  return pairs;
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
  if (frame->objtype != list_obj || env->objtype != list_obj) return NULL;

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
  obj* wrapper = new_list();
  list_of(wrapper)->car = o;
  return wrapper;
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

/**
 * Function: obj_cleanup
 * ---------------------
 * Cleanup an object given a pointer to a reference to the object. This
 * function was declared to use as the cleanup function for the CVector of
 * object references.
 * @param o_ref: Pointer to a pointer to the object to dispose of
 */
static void obj_cleanup(obj** o_ref) {
  dispose(*o_ref);
}
