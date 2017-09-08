/*
 * File: list.c
 * ------------
 * Implements the list data structure basic operations
 */

#include <list.h>

static void* getContents(obj* o);

list_t* getList(obj* o) {
  return (list_t*) getContents(o);
}

atom_t getAtom(obj* o) {
  return (atom_t) getContents(o);
}

primitive_t getPrimitive(obj* o) {
  return (primitive_t) getContents(o);
}

obj* copy(obj* o) {
  if (o == NULL) return NULL;

}

void dispose(obj* o) {
  if (o->objtype == list_obj) {
    list_t *l = getList(o);
    dispose(l->car);
    dispose(l->cdr);
  }
  free(o);
}

static void* getContents(obj* o) {
  return (void*) ((char*) o + sizeof(obj));
}