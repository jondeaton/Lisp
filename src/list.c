/*
 * File: list.c
 * ------------
 * Implements the list data structure along with basic operations
 * such as copying and and disposal
 */

#include <list.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <closure.h>
#include <primitives.h>

#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* copy_list_recursive(const obj *o);
static obj* copy_primitive(const obj* o);

obj* new_list_set(obj* car, obj* cdr) {
  obj* list = new_list();
  CAR(list) = car;
  CDR(list) = cdr;
  return list;
}

// Copy an object recursively
obj* copy_recursive(const obj *o) {
  if (!o) return NULL;

  // Different kind of copying for each object type
  if (is_atom(o))       return copy_atom(o);
  if (is_primitive(o))  return copy_primitive(o);
  if (is_list(o))       return copy_list_recursive(o);
  if (is_int(o))        return new_int(get_int(o));
  if (is_float(o))      return new_float(get_float(o));
  if (is_closure(o))    return copy_closure_recursive(o);
  return NULL;
}

void dispose_recursive(obj *o) {
  if (o == NULL) return;
  if (is_list(o)) { // Recursive disposal of lists and closures
    dispose_recursive(CAR(o));
    dispose_recursive(CDR(o));
  } else if (is_closure(o)) {
    dispose_recursive(PARAMETERS(o));
    dispose_recursive(PROCEDURE(o));
    dispose_recursive(CAPTURED(o));  }
  dispose(o);
}

bool is_empty(const obj* o) {
  if (o == NULL) return false;
  if (!is_list(o)) return false;
  return !CAR(o) && !CDR(o);
}

bool compare_recursive(const obj *x, const obj *y) {
  if (x == NULL) return y == NULL;
  if (x->objtype != y->objtype) return false;
  if (is_atom(x)) return strcmp(ATOM(x), ATOM(y)) == 0;
  if (is_primitive(x)) return PRIMITIVE(x) == PRIMITIVE(y);

  // List: cars must match and cdrs must match
  if (is_list(x))
    return compare_recursive(CAR(x), CAR(y)) && compare_recursive(CDR(x), CDR(y));
  else return x == y;
}

obj* ith(const obj* o, int i) {
  if (o == NULL || i < 0 || !is_list(o)) return NULL;
  FOR_LIST(o, x) {
    if (i == 0) return x;
    i--;
  }
  return NULL;
}

obj* sublist(const obj* o, int i) {
  if (!is_list(o)) return NULL;
  if (i == 0) return (obj*) o;
  return sublist(CDR(o), i - 1);
}

obj* join_lists(obj *list1, obj *list2) {
  if (!list1) return list2;
  if (!list2) return list1;
  if (!CDR(list1)) CDR(list1) = list2;
  else join_lists(CDR(list1), list2);
  return list1;
}

bool split_lists(obj *to_split, obj *second_list) {
  if (!to_split || !second_list) return false;
  if (to_split == second_list) return false;
  if (CDR(to_split) == second_list) {
    CDR(to_split) = NULL;
    return true;
  }
  else return split_lists(CDR(to_split), second_list);
}

int list_length(const obj* o) {
  if (o == NULL) return 0;
  if (!is_list(o)) return 1;
  int i = 0;
  for (const obj* l = o; l != NULL; l = CDR(l))
    i++;
  return i;
}

bool list_contains(const obj* list, const obj* query) {
  if (!query || !list) return false;
  FOR_LIST(list, el) {
    if (compare_recursive(el, query)) return true;
  }
  return false;
}

/**
 * Function: copy_list
 * -------------------
 * Make a deep copy of a list object. All elements that the list points to will be copied as well
 * @param o: An object that is a list to copy
 * @return: A pointer to a new list object
 */
static obj* copy_list_recursive(const obj *o) {
  if (!o) return NULL;
  obj* car = copy_recursive(CAR(o));
  obj* cdr = copy_recursive(CDR(o));
  return new_list_set(car, cdr);
}

/**
 * Function: copy_primitive
 * ------------------------
 * Copy a list object that contains a primitive
 * @param o: A list object that is a primitive
 * @return: A deep copy of the list object
 */
static obj* copy_primitive(const obj* o) {
  if (!o) return NULL;
  return new_primitive(*PRIMITIVE(o));
}
