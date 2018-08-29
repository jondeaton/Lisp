/*
 * File: lisp.h
 * ------------
 * Presents the interface to the lisp primitives. Th
 */

#ifndef _LISP_PRIMITIVES_H_INCLUDED
#define _LISP_PRIMITIVES_H_INCLUDED

#include "lisp-objects.h"
#include "memory-manager.h"
#define whatduhar __attribute__ ((unused))
/**
 * Primitive Definition Macro
 * --------------------------
 * Define a new primitive lisp procedure.
 *
 * @param name: The name of the primitive function
 * @param args: Lisp object containing the list of arguments to the primitive
 * @param envp: Pointer to the environment reference in which to evaluate the primitive
 * by applying it to the argument list passed in args
 * @param mm: Memory manager reference to store dynamically allocated lisp objects
 * that were created during the application of the primitive
 * @return: A new lisp object which is the result of applying
 */
#define def_primitive(name) obj *name(const obj *args whatduhar, obj **envp whatduhar, MemoryManager *mm whatduhar)

// Function type definitions
typedef obj*(*primitive_t)(const obj*, obj**, MemoryManager*);

/**
 * Function: get_primitive_env
 * ---------------------------
 * Get the library of primitive operations
 * @return: An environment constructed with the primitive operations
 */
obj *get_primitive_library();

/**
 * Function: new_primitive
 * -----------------------
 * Create a new primitive lisp object.
 * @param primitive: The primitive to wrap in an object
 * @return; The new primitive object wrapping the raw primitive instruction pointer
 */
obj *new_primitive(primitive_t primitive);

/**
 * Function: t
 * -----------
 * Get the truth atom. This is defined to be a lisp object of type atom with
 * the contents being the C-string "t". This object will be in dynamically allocated
 * memory and must be freed
 * @return: A pointer to a new truth atom in dynamically allocated memory
 */
obj *t(MemoryManager *mm);

/**
 * Function: nil
 * ---------------
 * Get an empty list. This is defined to be a lisp object of type lisp with
 * NULL for both car and cdr. This object will be in newly dynamically allocated
 * memory and must be freed
 * @return: A pointer to the a new empty list in dynamically allocated memory
 */
obj *nil(MemoryManager *mm);

#endif // _LISP_PRIMITIVES_H_INCLUDED
