/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include "evaluator.h"
#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* put_into_list(obj *o);
static obj* lookup(obj* o, obj* env);
static obj* bind(obj* params, obj* args, obj* env);
static obj* associate(obj* names, obj* values);
static obj* push_frame(obj *frame, obj *env);
static obj* eval_list(obj *list, obj *env);

// Evaluate a lisp expression
obj* eval(obj* o, obj* env) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (o->objtype == atom_obj) return lookup(o, env);

  // Primitive just means that there's nothing left to apply
  if (o->objtype == primitive_obj) return o;

  // List type means its a operator being applied to operands
  // which means evaluate the operator (return a procedure or a primitive)
  // to which we call apply on the arguments
  if (o->objtype == list_obj) {
    obj* operator = eval(get_list(o)->car, env);
    return apply(operator, get_list(o)->cdr, env);
  }
  else return NULL;
};

// Apply a closure object to a list of arguments
obj* apply(obj* closure, obj* args, obj* env) {
  if (closure == NULL) return NULL;

  if (closure->objtype == primitive_obj) {
    primitive_t f = *get_primitive(closure);
    return f(args, env);
  }

  if (closure->objtype == closure_obj) {
    obj* evaluatedArguments = eval_list(args, env);
    obj* params = get_list(get_list(closure)->cdr)->car;
    obj* newEnv = bind(params, evaluatedArguments, env);
    obj* exp = get_list(get_list(closure)->cdr)->cdr;
    return eval(exp, newEnv);
  }
  else return NULL;
}

/**
 * Function: lookup
 * ----------------
 * Looks up an object in an environment
 * @param o : A lisp object that is of the atom type
 * @param env : An environment to lookup the atom in
 * @return : The lisp object that was associated with the object in the environment
 */
static obj* lookup(obj* o, obj* env) {
  if (o == NULL || env == NULL) return NULL;

  // Error: The environment should be a list
  if (env->objtype != list_obj) return NULL;

  // Error: Can't lookup something that isn't an atom
  if (o->objtype != atom_obj) return NULL;

  // Get the list
  obj* pair = get_list(env)->car;


  if (strcmp(get_atom(o), get_atom(get_list(pair)->car)) == 0)
    return get_list(get_list(pair)->cdr)->car;

  else return lookup(o, get_list(env)->cdr);
}

/**
 * Function: bind
 * ---------------
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
 * Function: evalList
 * ------------------
 * Evaluate a list, creating a new list with the evaluated arguments
 * @param list : A lisp object that is a list to evaluate each element of
 * @param env : Environment to evaluate each element of the list
 * @return : A new list with the evaluated values of the passed list
 */
static obj* eval_list(obj *list, obj *env) {
  if (list == NULL) return NULL;
  if (list->objtype != list_obj) return NULL;
  obj* o = put_into_list(eval(get_list(list)->car, env));
  get_list(o)->cdr = eval_list(get_list(list)->cdr, env);
  return o;
}

/**
 * Function: associate
 * -------------------
 * Takes a list of variable names and pairs them up with values in a list of pairs
 * @param names : List of names to associate with values
 * @param values : List of values each associated with the name in the name list
 * @return : A list containing pairs of name-value pairs
 */
static obj* associate(obj* names, obj* values) {
  if (names == NULL || values == NULL) return NULL;
  if (names->objtype != list_obj || values->objtype != list_obj) return NULL;

  obj* nameCopy = copy(get_list(names)->car);
  obj* valueCopy = copy(get_list(values)->car);

  obj* pair = put_into_list(nameCopy);
  get_list(pair)->cdr = valueCopy;

  obj* pairs = put_into_list(pair);
  get_list(pairs)->cdr = associate(get_list(names)->cdr, get_list(values)->cdr);
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

  if (get_list(frame)->cdr == NULL) get_list(frame)->cdr = env;
  else push_frame(get_list(frame)->cdr, env);
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
  obj* listObj = calloc(1, sizeof(obj) + sizeof(list_t));
  if (listObj == NULL) return NULL;
  listObj->objtype = list_obj;
  get_list(listObj)->car = o;
  return listObj;
}