/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure along with basic operations
 * such as copying and and disposal
 */

#ifndef _LIST_H_INCLUDED
#define _LIST_H_INCLUDED

#include <lisp-objects.h>
#include <stdbool.h>

/**
 * Function: new_list_set
 * ----------------------
 * Creates a new list and sets the value of car and cdr
 * @param car: The value to set in the car of the new list
 * @param cdr: The value to set in the cdr of the new list
 * @return: The new list with the specified values set
 */
obj* new_list_set(obj* car, obj* cdr);

/**
 * Function: copy_recursive
 * ------------------------
 * Copies an object, returning a new one, leaving the old one untouched
 * @param o: An object to copy
 * @return: A copy of the object
 */
obj* copy_recursive(const obj *o);

/**
 * Function: dispose_recursive
 * ---------------------------
 * Free the allocated memory used to store this lisp object, recursing
 * on any child lisp objects in the case that the object is of the list type.
 * @param o: Pointer to the lisp object to dispose of recursively
 */
void dispose_recursive(obj *o);

/**
 * Function: is_empty
 * ------------------
 * @param o: A lisp object to determine if it is the empty list
 * @return: True if the object is the empty list, false otherwise
 */
bool is_empty(const obj* o);

/**
 * Function: deep_compare
 * ----------------------
 * Deep comparison of two lisp objects
 * @param x: The first object to compare
 * @param y: The second object to compare
 * @return: True if the two objects are identical, false otherwise
 */
bool deep_compare(obj* x, obj* y);

/**
 * Function: ith
 * -------------
 * Get the i'th element of a list
 * @param o: The list to get the i'th element of
 * @param i: The index (starting at 0) of the element to get
 * @return: Pointer to the element of the list at index i, or NULL if there is no element there
 */
obj* ith(const obj* o, int i);

/**
 * Function: list_length
 * ---------------------
 * Finds the length of a list object
 * @param o: The list to get the length of
 * @return: The number of elements in a list
 */
int list_length(const obj* o);

#endif // _LIST_H_INCLUDED
