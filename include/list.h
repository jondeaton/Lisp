/*
 * File: list.h
 * ------------
 * Exports the interface to the list data structure along with basic operations
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

obj* new_list_set(const obj *car, const obj *cdr);
bool is_nil(const obj *o);
bool compare_recursive(const obj *x, const obj *y);
obj* ith(const obj* o, int i);
obj* sublist(const obj* o, int i);
obj* join_lists(obj *list1, obj *list2);
int list_length(const obj* o);

#endif // _LIST_H_INCLUDED
