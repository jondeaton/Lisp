/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include <lisp.h>
#include <string.h>
#include <assert.h>
#include <list.h>


// Static function declarations
static bool cmp(obj* x, obj* y);
static obj* putIntoList(obj* o);

obj t_atom = {
  .objtype = atom_obj
};

obj empty_atom = {
  .objtype = atom_obj
};

obj* t = &t_atom;
obj* empty = &empty_atom;

obj* eval(obj* o, obj* env) {
  if (o == NULL) return NULL;
  if (o->objtype == atom_obj) return o;
  if (o->objtype == list_obj) {
    obj* result = putIntoList(car(o));
    getList(result)->cdr = cdr(o);
    return o;
  }
  if (o->objtype == primitive_obj || o->objtype == closure_obj)
    return apply(getList(o)->car, getList(o)->cdr, env);
  return NULL;
};

obj* apply(obj* closure, obj* args, obj* env) {
  if (closure == NULL) return NULL;

  if (closure->objtype == primitive_obj) {
    primitive_t f = getPrimitive(closure);
    return f(eval(args, env));
  }
  else if (closure->objtype == closure_obj) {

  }
  else return NULL;
}


obj* quote(obj* o) {
  if (o == NULL) return NULL;
  return o;
}

obj* atom(obj* o) {
  if (o == NULL) return NULL;
  return o->objtype == atom_obj ? t : empty;
}

obj* eq(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  return cmp(car(o), cdr(o)) ? t : empty;
}

obj* car(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = getList(o);
  return eval(l->car);
}

obj* cdr(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = getList(o);
  return eval(l->cdr);
}

obj* cons(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);

  obj* x = car(o);
  obj* y = cdr(o);

  obj* new_obj = calloc(1, sizeof(obj) + sizeof(list_t));
  new_obj->objtype = list_obj;
  list_t* l = getList(new_obj);
  l->car = x;
  l->cdr = y;
  return new_obj;
}

obj* cond(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = getList(o);

  while (true) {
    obj* pair_obj = l->car;
    assert(pair_obj->objtype == list_obj);
    list_t* pair = getList(pair_obj);
    if (eval(pair->car) == t) return pair->cdr;

    if (l->cdr == NULL) return empty;
    assert(l->cdr->objtype == list_obj);
    l = getList(l->cdr);
  }
}

/**
 * Function: cmp
 * -------------
 * Comparison of two lisp objects
 * @param x : Pointer to the first lisp object
 * @param y : Pointer to the second lisp object
 * @return : True if the two are equal, false otherwise
 */
static bool cmp(obj* x, obj* y) {
  if (x->objtype != y->objtype) return false;
  if (x->objtype == atom_obj) return strcmp(getAtom(x), getAtom(y)) == 0;
  if (x->objtype == list_obj) return cmp(car(x), car(y)) && cmp(cdr(x), cdr(y));
  else return x == y;
}

/**
 * Function: putIntoList
 * ---------------------
 * Makes a list object with car pointing to the object passed
 * @param o : The object that the list's car should point to
 * @return : A pointer to the list object containing only the argument object
 */
static obj* putIntoList(obj* o) {
  obj* listObj = calloc(1, sizeof(obj) + sizeof(list_t));
  if (listObj == NULL) return NULL;
  listObj->objtype = list_obj;
  getList(listObj)->car = o;
  return listObj;
}