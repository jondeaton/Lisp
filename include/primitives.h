/*
 * File: lisp.h
 * ------------
 * Presents the interface to the lisp primitives. Th
 */

#ifndef _LISP_PRIMITIVES_H_INCLUDED
#define _LISP_PRIMITIVES_H_INCLUDED

#include "lisp-objects.h"
#include "garbage-collector.h"

typedef obj*(*primitive_t)(const obj*, obj**, GarbageCollector*);

/**
 * Function: new_primitive
 * -----------------------
 * Create a new primitive object
 * @param primitive: The primitive to wrap in an object
 * @return; The new primitive object wrapping the raw primitive instruction pointer
 */
obj* new_primitive(primitive_t primitive);

/**
 * Function: get_primitive
 * -----------------------
 * Gets a pointer to a function pointer to the primitive function for a primitive object list
 * @param o: Pointer to a lisp data structure
 * @return: Function pointer to primitive function
 */
primitive_t* primitive_of(const obj *o);

/**
 * Function: get_primitive_env
 * ---------------------------
 * Get the library of primitive operations
 * @return: An environment constructed with the primitive operations
 */
obj* get_primitive_library();

/**
 * Function: t
 * -----------
 * Get the truth atom. This is defined to be a lisp object of type atom with
 * the contents being the C-string "t". This object will be in dynamically allocated
 * memory and must be freed
 * @return: A pointer to a new truth atom in dynamically allocated memory
 */
obj *t(GarbageCollector *gc);

/**
 * Function: empty
 * ---------------
 * Get an empty list. This is defined to be a lisp object of type lisp with
 * NULL for both car and cdr. This object will be in newly dynamically allocated
 * memory and must be freed
 * @return: A pointer to the a new empty list in dynamically allocated memory
 */
obj *empty(GarbageCollector *gc);

/**
 * Primitive: quote
 * ----------------
 * Returns the unevaluated version of the object
 * @param args: The object to quote
 * @param envp: The environment to evaluate this primitive in
 * @return: Pointer to the lisp object without evaluating it
 */
obj *quote(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: atom
 * ---------------
 * Checks if an object is an atom
 * @param args: An object to check if it is an atom
 * @param env: The environment to evaluate this primitive in
 * @return: t if it is an atom, else the empty list
 */
obj *atom(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: eq
 * -------------
 * Test for equality of two objects
 * @param args: pointer to a list containing the first object and then the second object
 * @param env : The environment to evaluate this primitive in
 * @return t if both are equal, empty list (false) otherwise
 */
obj *eq(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: car
 * -------------
 * Expects the value of l to be a list and returns it's first element
 * @param args : A list object (will be asserted)
 * @param env : The environment to evaluate this primitive in
 * @return : The first element of the list
 */
obj *car(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: cdr
 * -------------
 * Expects the value of l to be a list and returns everything after the first element
 * @param args: A list object (will be asserted)
 * @param env: The environment to evaluate this primitive in
 * @return: Everything after the first element of the list
 */
obj *cdr(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: cons
 * ---------------
 * Expects the value of y to be a list and returns a list containing the value
 * of x followed by the elements of the value of y 
 * @param args: the argument to the cons call
 * @param env: The environment to evaluate this primitive in
 * @return: Pointer to the concatenated list
 */
obj *cons(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: cond
 * ---------------
 * (cond (p1 e1) ... (pn en))
 * The p expressions are evaluated in order until one returns t
 * When one is found  the value of the corresponding e expression
 * is returned as the expression
 * @param o: The argument to the cond call
 * @param env: The environment to evaluate this primitive in
 * @return: Pointer to the evaluation of expression the predicated of which evaluated to true
 */
obj *cond(const obj *o, obj **envp, GarbageCollector *gc);

/**
 * Primitive: set
 * --------------
 * Primitive function for setting a value in the environment that the
 * primitive is evaluated in
 * Usage: (set 'foo 42)
 * @param args: The pointer to the argument to the set function
 * @param env: The environment to evaluate this primitive in (will be modified!)
 * @return: todo
 */
obj *set(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: env
 * --------------
 * Simply returns the environment
 * @param args: Unused arguments
 * @param env: The environment to evaluate
 * @return: The environment object (unmodified)
 */
obj *env_prim(const obj *args, obj **envp, GarbageCollector *gc);

/**
 * Primitive: defmacro
 * -------------------
 * Defines a macro
 * @param args: The pointer to the argument list to defmacro
 * @param env: The environment to define the macro in
 * @return: todo
 */
obj *defmacro(const obj *args, obj **envp, GarbageCollector *gc);

#endif // _LISP_PRIMITIVES_H_INCLUDED
