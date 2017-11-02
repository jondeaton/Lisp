/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include "evaluator.h"
#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* putIntoList(obj* o);
static obj* lookup(obj* o, obj* env);
static obj* bind(obj* params, obj* args, obj* env);
static obj* associate(obj* names, obj* values);
static obj* pushFrame(obj* frame, obj* env);
static obj* evalList(obj* list, obj* env);

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
    obj* operator = eval(getList(o)->car, env);
    return apply(operator, getList(o)->cdr, env);
  }
  else return NULL;
};

// Apply a closure object to a list of arguments
obj* apply(obj* closure, obj* args, obj* env) {
  if (closure == NULL) return NULL;

  if (closure->objtype == primitive_obj) {
    primitive_t f = *getPrimitive(closure);
    return f(args, env);
  }

  if (closure->objtype == closure_obj) {
    obj* evaluatedArguments = evalList(args, env);
    obj* params = getList(getList(closure)->cdr)->car;
    obj* newEnv = bind(params, evaluatedArguments, env);
    obj* exp = getList(getList(closure)->cdr)->cdr;
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
  obj* pair = getList(env)->car;


  if (strcmp(getAtom(o), getAtom(getList(pair)->car)) == 0)
    return getList(getList(pair)->cdr)->car;

  else return lookup(o, getList(env)->cdr);
}

/**
 * Function: bind
 * ---------------
 * binds a list of arguments to parameters and pushes them onto an environment
 * @param params : Parameters
 * @param args : Arguments
 * @param env : Environment
 * @return : Environment now with bound arguments appended
 */
static obj* bind(obj* params, obj* args, obj* env) {
  obj* frame = associate(params, args);
  return pushFrame(frame, env);
}

/**
 * Function: evalList
 * ------------------
 * Evaluate a list, creating a new list with the evaluated arguments
 * @param list : A lisp object that is a list to evaluate each element of
 * @param env : Environment to evaluate each element of the list
 * @return : A new list with the evaluated values of the passed list
 */
static obj* evalList(obj* list, obj* env) {
  if (list == NULL) return NULL;
  if (list->objtype != list_obj) return NULL;
  obj* o = putIntoList(eval(getList(list)->car, env));
  getList(o)->cdr = evalList(getList(list)->cdr, env);
  return o;
}

/**
 * Function: associate
 * -----------------------
 * Takes a list of variable names and pairs them up with values in a list of pairs
 * @param names : List of names to associate with values
 * @param values : List of values each associated with the name in the name list
 * @return : A list containing pairs of name-value pairs
 */
static obj* associate(obj* names, obj* values) {
  if (names == NULL || values == NULL) return NULL;
  if (names->objtype != list_obj || values->objtype != list_obj) return NULL;

  obj* nameCopy = copy(getList(names)->car);
  obj* valueCopy = copy(getList(values)->car);

  obj* pair = putIntoList(nameCopy);
  getList(pair)->cdr = valueCopy;

  obj* pairs = putIntoList(pair);
  getList(pairs)->cdr = associate(getList(names)->cdr, getList(values)->cdr);
  return pairs;
}

/**
 * Function: pushFrame
 * -------------------
 * Pushes a list of pairs of associated keys and values onto a list of such pairs
 * @param frame : List of name-value pairs
 * @param env : List of name-value pairs to append the frame to
 * @return : The new augmented list of pairs, with the frame on the front
 */
static obj* pushFrame(obj* frame, obj* env) {
  if (frame == NULL) return env;
  if (env == NULL) return frame;
  if (frame ->objtype != list_obj || env->objtype != list_obj) return NULL;

  if (getList(frame)->cdr == NULL) getList(frame)->cdr = env;
  else pushFrame(getList(frame)->cdr, env);
  return frame;
}

/**
 * Function: putIntoList
 * ---------------------
 * Makes a list object with car pointing to the object passed
 * @param o : The object that the list's car should point to
 * @return : A pointer to the list object containing only the argument object
 */
static obj* putIntoList(obj* o) {
  obj* listObj = calloc(1, sizeof(obj) + sizeof(list_t));
  if (listObj == NULL) return NULL;
  listObj->objtype = list_obj;
  getList(listObj)->car = o;
  return listObj;
}