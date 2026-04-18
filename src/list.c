/*
 * File: list.c
 * ------------
 * Implements the list data structure along with basic operations
 */

#include <list.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <closure.h>
#include <primitives.h>

#include <stdlib.h>
#include <string.h>

obj* new_list_set(const obj *car, const obj *cdr) {
  obj* list = new_list();
  CAR(list) = (obj *) car;
  CDR(list) = (obj *) cdr;
  return list;
}

obj* copy_recursive(const obj *o) {
  if (!o) return NULL;

  if (is_atom(o))      return new_atom(ATOM(o));
  if (is_int(o))       return new_int(get_int(o));
  if (is_float(o))     return new_float(get_float(o));
  if (is_primitive(o)) return new_primitive(PRIMITIVE(o));
  if (is_closure(o)) {
    obj *params = copy_recursive(PARAMETERS(o));
    obj *proc = copy_recursive(PROCEDURE(o));
    obj *capt = copy_recursive(CAPTURED(o));
    return new_closure_set(params, proc, capt);
  }
  if (is_list(o)) {
    obj* car = copy_recursive(CAR(o));
    obj* cdr = copy_recursive(CDR(o));
    return new_list_set(car, cdr);
  }
  return NULL;
}

bool is_nil(const obj *o) {
  if (o == NULL) return false;
  if (!is_list(o)) return false;
  return !CAR(o) && !CDR(o);
}

bool compare_recursive(const obj *x, const obj *y) {
  if (x == NULL) return y == NULL;
  if (x->objtype != y->objtype) return false;
  if (is_atom(x)) return strcmp(ATOM(x), ATOM(y)) == 0;
  if (is_primitive(x)) return PRIMITIVE(x) == PRIMITIVE(y);

  if (is_list(x))
    return compare_recursive(CAR(x), CAR(y)) && compare_recursive(CDR(x), CDR(y));
  else return x == y;
}

obj* ith(const obj* o, int i) {
    if (o == NULL || i < 0 || !is_list(o)) return NULL;
    if (i == 0) return CAR(o);
    return ith(CDR(o), i - 1);
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

int list_length(const obj* o) {
  if (o == NULL) return 0;
  if (!is_list(o)) return 1;
  int i = 0;
  for (const obj* l = o; l != NULL; l = CDR(l))
    i++;
  return i;
}

bool list_contains(const obj* list, const obj* query) {
  if (list == NULL || query == NULL) return false;
  if (compare_recursive(CAR(list), query)) return true;
  return list_contains(CDR(list), query);
}
