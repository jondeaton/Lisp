/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _LISP_ENV_H
#define _LISP_ENV_H

#include "primitives.h"

/**
 * Function: init_env
 * ------------------
 * Initializes the default lisp environment
 * @return: Pointer to lisp environment object
 */
obj* init_env();

/**
 * Function: make_environment
 * --------------------------
 * Make an environment from an array of primitive names and an array of corresponding primitive functions
 * @param primitive_names: Array of primitive names
 * @param primitives: Array of corresponding primitive functions
 * @return: An environment object made form pairing the names with the primitive functions
 */
obj* make_environment(const atom_t primitive_names[], const primitive_t primitives[]);

/**
 * Function: lookup
 * ----------------
 * Looks up an object in an environment
 * @param o: A lisp object that is of the atom type
 * @param env: An environment to lookup the atom in
 * @return: The lisp object that was associated with the object in the environment
 */
obj* lookup(const obj* o, const obj* env);

/**
 * Function: lookup_entry
 * ----------------------
 * Looks up an object in an environment, returning a pointer to the place in the environment
 * that points to the value that is equal to
 * @param o: A lisp object that is of the atom type
 * @param env: An environment to lookup the atom in
 * @return: A pointer to obj* inside the environment that
 */
obj** lookup_entry(const obj* o, const obj* env);

/**
 * Function: make_pair
 * -------------------
 * Associate a name with a value in a two-item list (pair) by copying the name and the
 * value into a new list. Both the copies and the list are in newly dynamically allocated memory
 * @param name: A name to associate with a value (will be copied)
 * @param value: The value to associate with a name (will be copied)
 * @return: A list of length two where the first element is the copy of the name and the second element
 * is a copy of the value.
 */
obj* make_pair(const obj* name, const obj* value);

#endif // _LISP_ENV_H