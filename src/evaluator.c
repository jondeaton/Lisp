/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include "evaluator.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* put_into_list(obj *o);
static obj* lookup(const obj* o, const obj* env);
static obj* bind(obj* params, obj* args, obj* env);
static obj* associate(obj* names, obj* values);
static obj* push_frame(obj *frame, obj *env);
static obj* eval_list(const obj* list, obj *env);

// Evaluate a lisp expression
obj* eval(const obj* o, obj* env) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (o->objtype == atom_obj) return lookup(o, env);

  // Primitive just means that there's nothing left to apply
  if (o->objtype == primitive_obj) return (obj*) o;

  // List type means its a operator being applied to operands
  // which means evaluate the operator (return a procedure or a primitive)
  // to which we call apply on the arguments
  if (o->objtype == list_obj) {
    if (is_empty(o)) return (obj*) o;
    obj* operator = eval(list_of(o)->car, env);
    return apply(operator, list_of(o)->cdr, env);
  }
  else return NULL;
};

// Apply a closure object to a list of arguments
obj* apply(const obj* closure, const obj* args, obj* env) {
  if (closure == NULL) return NULL;
  if (closure->objtype == atom_obj) return NULL;

  if (closure->objtype == primitive_obj) {
    primitive_t prim = *primitive_of(closure);
    return prim(args, env);

  } else {
    obj* arg_values = eval_list(args, env);
    obj* params = list_of(list_of(closure)->cdr)->car;
    obj* newEnv = bind(params, arg_values, env);
    obj* exp = list_of(list_of(closure)->cdr)->cdr;
    return eval(exp, newEnv);
  }
}

/**
 * Function: lookup
 * ----------------
 * Looks up an object in an environment
 * @param o : A lisp object that is of the atom type
 * @param env : An environment to lookup the atom in
 * @return : The lisp object that was associated with the object in the environment
 */
static obj* lookup(const obj* o, const obj* env) {
  if (o == NULL || env == NULL) return NULL;

  // Error: The environment should be a list
  if (env->objtype != list_obj) return NULL;

  // Error: Can't lookup something that isn't an atom
  if (o->objtype != atom_obj) return NULL;

  // Get the list
  obj* pair = list_of(env)->car;

  if (strcmp(atom_of(o), atom_of(list_of(pair)->car)) == 0)
    return list_of(list_of(pair)->cdr)->car;

  else return lookup(o, list_of(env)->cdr);
}

/**
 * Function: bind
 * --------------
 * binds a list of arguments to parameters and pushes them onto an environment
 * @param params: Parameters
 * @param args: Arguments
 * @param env: Environment
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

  obj* nameCopy = copy(list_of(names)->car);
  obj* valueCopy = copy(list_of(values)->car);

  obj* pair = put_into_list(nameCopy);
  list_of(pair)->cdr = valueCopy;

  obj* pairs = put_into_list(pair);
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
static obj* push_frame(obj *frame, obj *env) {
  if (frame == NULL) return env;
  if (env == NULL) return frame;
  if (frame ->objtype != list_obj || env->objtype != list_obj) return NULL;

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