/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure along with basic operations
 * such as copying and and disposal
 */

#ifndef _LIST_H_INCLUDED
#define _LIST_H_INCLUDED

#include "lisp-objects.h"
#include <stdbool.h>

// For loop for iterating through elements of a list
#define FOR_LIST(_list, _el) const obj* _l = _list; \
  for (obj* (_el) = _l != NULL ? CAR(_l) : NULL; \
  _l != NULL; \
  _l = CDR(_l), (_el) = _l != NULL ? CAR(_l) : NULL)
    
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
bool is_nil(const obj *o);

/**
 * Function: compare_recursive
 * ---------------------------
 * Deep comparison of two lisp objects
 * @param x: The first object to compare
 * @param y: The second object to compare
 * @return: True if the two objects are identical, false otherwise
 */
bool compare_recursive(const obj *x, const obj *y);

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
 * Function: sublist
 * -----------------
 * Gets the elements of a list after (inclusive) a specified index
 * @param o: The list to get the sublist of
 * @param i: The index to get elements after (and including)
 * @return: The list starting at index i
 */
obj* sublist(const obj* o, int i);

/**
 * Function: join_lists
 * ---------------------
 * Prepends one list to another at their top levels
 * @param list1: The list to prepend to list2
 * @param list2: The list to append to list1
 * @return: The first list, now with it's end pointing to the start of list2
 */
obj* join_lists(obj *list1, obj *list2);

/**
 * Function: split_lists
 * ---------------------
 * Splits a list into two lists at a specified location
 * @param to_split: The list to split
 * @param second_list: Pointer to the start of the second list, where the list should be split.
 */
bool split_lists(obj *to_split, obj *second_list);

/**
 * Function: list_length
 * ---------------------
 * Finds the length of a list object
 * @param o: The list to get the length of
 * @return: The number of elements in a list
 */
int list_length(const obj* o);

/**
 * Function: list_contains
 * -----------------------
 * Determines if a list contains a specified query at the top level. Although this function compares
 * the query recursively to each element of the list, the search will not recurse down to lower levels of the tree
 * @param list: The list to search for the query
 * @param query: The query to search for
 * @return: True if the query matches some element of the list
 */
bool list_contains(const obj* list, const obj* query);

#endif // _LIST_H_INCLUDED
