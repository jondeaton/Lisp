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
#include <stdlib.h>
#include <string.h>

// Static function declarations
static obj* copy_list_recursive(const obj *o);
static obj* copy_primitive(const obj* o);

obj* new_list_set(obj* car, obj* cdr) {
  obj* list = new_list();
  list_of(list)->car = car;
  list_of(list)->cdr = cdr;
  return list;
}

// Copy an object recursively
obj* copy_recursive(const obj *o) {
  if (o == NULL) return NULL;

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
    list_t *l = list_of(o);
    dispose_recursive(l->car);
    dispose_recursive(l->cdr);
  } else if (is_closure(o)) {
    closure_t* closure = closure_of(o);
    dispose_recursive(closure->parameters);
    dispose_recursive(closure->procedure);
    dispose_recursive(closure->captured);
  }
  dispose(o);
}

bool is_empty(const obj* o) {
  if (o == NULL) return false;
  if (!is_list(o)) return false;
  return list_of(o)->car == NULL && list_of(o)->cdr == NULL;
}

bool compare_recursive(const obj *x, const obj *y) {
  if (x == NULL) return y == NULL;
  if (x->objtype != y->objtype) return false;
  if (is_atom(x)) return strcmp(atom_of(x), atom_of(y)) == 0;
  if (is_primitive(x)) return primitive_of(x) == primitive_of(y);

  // List: cars must match and cdrs must match
  if (is_list(x))
    return compare_recursive(list_of(x)->car, list_of(y)->car) && compare_recursive(list_of(x)->cdr, list_of(y)->cdr);
  else return x == y;
}

obj* ith(const obj* o, int i) {
  if (o == NULL || i < 0 || !is_list(o)) return NULL;
  if (i == 0) return list_of(o)->car;
  return ith(list_of(o)->cdr, i - 1);
}

obj* sublist(const obj* o, int i) {
  if (o == NULL || o->objtype != list_obj) return NULL;
  if (i == 0) return (obj*) o;
  return sublist(list_of(o)->cdr, i - 1);
}

obj* join_lists(obj *list1, obj *list2) {
  if (list1 == NULL) return list2;
  if (list2 == NULL) return list1;
  if (list_of(list1)->cdr == NULL) list_of(list1)->cdr = list2;
  else join_lists(list_of(list1)->cdr, list2);
  return list1;
}

bool split_lists(obj *to_split, obj *second_list) {
  if (to_split == NULL || second_list == NULL) return false;
  if (to_split == second_list) return false;
  if (list_of(to_split)->cdr == second_list) {
    list_of(to_split)->cdr = NULL;
    return true;
  }
  else return split_lists(list_of(to_split)->cdr, second_list);
}

int list_length(const obj* o) {
  if (o == NULL) return 0;
  if (!is_list(o)) return 1;
  return 1 + list_length(list_of(o)->cdr);
}

bool list_contains(const obj* list, const obj* query) {
  if (query == NULL || list == NULL) return false;
  bool found_here = compare_recursive(list_of(list)->car, query);
  return found_here || list_contains(list_of(list)->cdr, query);
}

/**
 * Function: copy_list
 * -------------------
 * Make a deep copy of a list object. All elements that the list points to will be copied as well
 * @param o: An object that is a list to copy
 * @return: A pointer to a new list object
 */
static obj* copy_list_recursive(const obj *o) {
  if (o == NULL) return NULL;
  obj* car = copy_recursive(list_of(o)->car);
  obj* cdr = copy_recursive(list_of(o)->cdr);
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
  if (o == NULL) return NULL;
  return new_primitive(*primitive_of(o));
}
