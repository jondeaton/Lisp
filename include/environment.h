/*
 * File: environment.h
 * -------------------
 * Exposes the default lisp environment interface
 */

#ifndef _ENVIRONMENT_H_INCLUDED
#define _ENVIRONMENT_H_INCLUDED

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
 * @param primitive_list: Array of corresponding primitive functions
 * @return: An environment object made form pairing the names with the primitive functions
 */
obj* make_environment(atom_t const primitive_names[], const primitive_t primitive_list[]);

/**
 * Function: lookup
 * ----------------
 * Looks up an object in an environment, returning the value that is associated with that object
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
 * @param key: A lisp object that is of the atom type
 * @param env: An environment to lookup the atom in
 * @return: A pointer to obj* inside the environment that
 */
obj** lookup_entry(const obj* key, const obj* env);

/**
 * Function: lookup_pair
 * ---------------------
 * Looks up a key in an environment, returning the key-value pair if it is found
 * @param key: The key to search for in the environment
 * @param env: The environment to search for the key in
 * @return: The key-value pair from the environment with a matching key, if one was found else NULL
 */
obj* lookup_pair(const obj* key, const obj* env);

/**
 * Function: make_pair
 * -------------------
 * Associate a name with a value in a two-item list (pair) by copying the name and the
 * value into a new list. Both the copies and the list are in newly dynamically allocated memory
 * @param key: A name to associate with a value (will be copied)
 * @param value: The value to associate with a name (will be copied)
 * @param copy: If true, copy the key and value, and if false, make the pair using the provided key and pair pointers
 * @return: A list of length two where the first element is the copy of the name and the second element
 * is a copy of the value.
 */
obj *make_pair(obj *key, obj *value, bool copy);

#endif // _ENVIRONMENT_H_INCLUDED
