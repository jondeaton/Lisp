/**
 * File: garbage-collector.h
 * -------------------------
 * Presents the interface of the garbage collection system for the Lisp
 * interpreter. This garbage collection system works as follows:
 *
 * The interface provided here maintains a dynamically sized list of references to
 * objects in allocated memory that need to be disposed of once
 * they are no longer in use by the interpreter. The objects added to this list
 * should be those which are created (allocated) during expression evaluation
 * (NOT those created during parsing), but which are not associated with
 * the environment (the lifetime of which is managed differently). An example is the
 * list cell allocated during the "cons" primitive. After the evaluation of
 * each expression, the entire list of objects will be disposed of. The idea here is that
 * any object who's lifetime is longer than the evaluation
 * of a single expression will have been copied into the environment at some point
 * before the end of the expression's evaluation. Therefore, all objects
 * created during evaluation, can be freed at the end leaving only the parsed
 * data structure code, and the environment.
 *
 * Note that garbage should be collected only AFTER the result of the evaluation has been
 * fully processed (e.g. serialized and printed) to ensure that no objects are destroyed
 * that are contained within the final result of the evaluation.
 */

#ifndef _LISP_GARBAGE_COLLECTOR_H_INCLUDED
#define _LISP_GARBAGE_COLLECTOR_H_INCLUDED

#include "lisp-objects.h"

typedef struct GarbageCollectorImpl GarbageCollector;

/**
 * Function: gc_init
 * ------------------------
 * Initializes a dynamic list of allocated object references. Call this before any calls
 * to eval are made. Calling eval without first calling this method results in
 * undefined behavior.
 */
GarbageCollector * gc_init();

/**
 *  Function: gc_add
 * -----------------------
 * Add an object to the list of objects that need to be freed at the end
 * of expression evaluation. This method should be called by any function
 * internal to eval that allocates Lisp objects in dynamically allocated memory.
 * @param o: A pointer to a lisp object that needs to be free'd
 */
void gc_add(GarbageCollector *gc, const obj *o);

/**
 * Function: gc_add_recursive
 * ---------------------------------
 * Add an object to the list of objects that need to be freed at the end
 * of expression evaluation, including all of the objects that is references
 * in a recursive manner (the entire object tree).
 * @param root: The root object to add to the list
 */
void gc_add_recursive(GarbageCollector *gc, const obj *root);

/**
 * Function: gc_clear
 * -------------------------
 * Frees all of the allocated lisp objects in the allocated list. Suggested usage is to
 * call this function after each call to repl_eval, after the object returned from
 * eval has been completely processed (e.g. copied into environment, serialized, etc...).
 */
void gc_clear(GarbageCollector *gc);

/**
 * Function: gc_dispose
 * ---------------------------
 * Disposes of the CVector of allocated objects. Call this method after
 * all calls to eval are completed to free the memory used to store the
 * CVector of allocated objects.
 */
void gc_dispose(GarbageCollector *gc);

#endif //_LISP_GARBAGE_COLLECTOR_H_INCLUDED
