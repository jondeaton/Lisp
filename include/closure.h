/*
 * File: closures.h
 */

#ifndef _CLOSURE_H_INCLUDED
#define _CLOSURE_H_INCLUDED

#include "lisp-objects.h"
#include "memory-manager.h"

/**
 * Function: closure_partial_application
 * -------------------------------------
 * Partially applies the arguments
 * @param closure: The closure to partially apply arguments to
 * @param args: The (unevaluated) argument list to apply to the closure
 * @return: A newly allocated closure with arguments bound
 */
obj *closure_partial_application(const obj *closure, const obj *args, obj **envp, MemoryManager *gc);

/**
 * Function: new_closure_set
 * -------------------------
 * Create a new closure with initialization of fields. The passed objects will not be copied.
 * @param params: Parameters to the closure (will not be copied)
 * @param procedure: Procedure of the closure (will not be copied)
 * @param captured: Captured argument list of the closure (will not be copied)
 * @return: A new closure object with the specified parameters, procedure, and captured vars list.
 */
obj *new_closure_set(obj *params, obj *procedure, obj *captured);

/**
 * Function: copy_closure_recursive
 * --------------------------------
 * Make a deep copy of a closure by recursively copying it's parameters, procedure,
 * and captured fields.
 * @param closure: The closure to make a copy of
 * @return: A completely newly allocated closure identical to the passed one
 */
obj* copy_closure_recursive(const obj* closure);

/**
 * Function: associate
 * -------------------
 * Takes a list of variable names and pairs them up with values in a list of pairs
 * @param names: List of names to associate with values
 * @param args: List of values each associated with the name in the name list
 * @param envp: Pointer to the environment to associate the
 * @return: A list of name-value pairs (names are copied)
 */
obj *associate(obj *names, const obj *args, obj **envp, MemoryManager *gc);

#endif // _CLOSURE_H_INCLUDED
