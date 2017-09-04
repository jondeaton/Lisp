/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include <lisp.h>
#include <string.h>
#include <assert.h>

// Static function declarations
static bool cmp(obj* x, obj* y);
static obj* apply(obj* fo, obj* args);

obj t_atom = {
  .objtype = atom_obj,
  .p = "t"
};

obj empty_atom = {
  .objtype = atom_obj,
  .p = "()"
};

obj* t = &t_atom;
obj* empty = &empty_atom;

// Reduce a list
obj* eval(obj* o) {
  if (o == NULL) return NULL;

  if (o->objtype == atom_obj) return o;
  if (o->objtype == list_obj) {
    list_t* l = o->p;
    return apply(l->car, l->cdr);
  }
  return NULL;
};

void dispose(obj* o) {
  if (o->objtype == list_obj) {
    list_t* l = o->p;
    dispose(l->car);
    dispose(l->cdr);
    free(l);
  } else if (o->objtype == atom_obj) {
    free(o->p);
  }
  free(o);
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
  list_t* l = o->p;
  return eval(l->car);
}

obj* cdr(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = o->p;
  return eval(l->cdr);
}

obj* cons(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);

  obj* x = car(o);
  obj* y = cdr(o);

  obj* new_obj = calloc(1, sizeof(obj));
  new_obj->objtype = list_obj;
  list_t* l = calloc(1, sizeof(list_t));
  l->car = x;
  l->cdr = y;
  new_obj->p = l;
  return new_obj;
}

obj* cond(obj* o) {
  if (o == NULL) return NULL;
  assert(o->objtype == list_obj);
  list_t* l = o->p;

  while (true) {
    obj* pair_obj = l->car;
    assert(pair_obj->objtype == list_obj);
    list_t* pair = pair_obj->p;
    if (eval(pair->car) == t) return pair->cdr;

    if (l->cdr == NULL) return empty;
    assert(l->cdr->objtype == list_obj);
    l = l->cdr->p;
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
  if (atom(x) == t && atom(y))
    return strcmp(x->p, y->p) == 0;
  else return x->p == y->p;
}

/**
 * Function: apply
 * ---------------
 * Applies a lisp function to an argument list
 * @param o : A lisp function object (will be asserted)
 * @param args : List of arguments
 * @return : The result of the application of the function to the arguments
 */
static obj* apply(obj* fo, obj* args) {
  if (fo == NULL) return NULL;

  if (fo->objtype == primitive_obj) {
    primitive_t f = fo->p;
    return f(eval(args));
  }

  else if (fo->objtype == func_obj)
    return NULL;

  else return NULL;
}