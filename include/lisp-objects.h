/*
 * File: lisp-objects.h
 * --------------------
 * Presents the interface of lisp object type definitions and creation of objects
 */

#ifndef _LISP_OBJECTS_H_INCLUDED
#define _LISP_OBJECTS_H_INCLUDED

#include <stdbool.h>

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

// The different types of objects
enum type {
  atom_obj,             // Atom object
  list_obj,             // List object
  primitive_obj,        // Primitive function object
  closure_obj,          // Closure/procedure object
  macro_obj,            // Macro object (like closure, but different application)
  int_obj,              // Integer object
  float_obj             // Floating point number object
};

typedef const char* atom_t;

// Forward declarations to break circular dependency
typedef struct obj obj;
typedef struct LispInterpreter LispInterpreter;

// Primitive function pointer type
typedef obj*(*primitive_t)(const obj*, LispInterpreter*);

/**
 * @struct obj
 * @brief Generic lisp object using a tagged union.
 */
struct obj {
  enum type objtype;
  bool reachable;       // for GC mark phase
  union {
    char *atom;                                                       // atom_obj
    struct { obj *car, *cdr; } list;                                  // list_obj
    primitive_t primitive;                                            // primitive_obj
    struct { obj *parameters, *procedure, *captured; int nargs; } closure;  // closure_obj
    int intval;                                                       // int_obj
    float floatval;                                                   // float_obj
  };
};

// Field access macros
#define ATOM(o)       ((o)->atom)
#define CAR(o)        ((o)->list.car)
#define CDR(o)        ((o)->list.cdr)
#define PRIMITIVE(o)  ((o)->primitive)
#define PARAMETERS(o) ((o)->closure.parameters)
#define PROCEDURE(o)  ((o)->closure.procedure)
#define CAPTURED(o)   ((o)->closure.captured)
#define NARGS(o)      ((o)->closure.nargs)

obj* new_atom(atom_t name);
obj* new_list(void);
obj* new_closure(void);
obj* new_int(int value);
obj* new_float(float value);

bool compare(const obj* a, const obj* b);

bool is_atom(const obj* o);
bool is_primitive(const obj* o);
bool is_list(const obj* o);
bool is_closure(const obj* o);
bool is_macro(const obj* o);
bool is_int(const obj* o);
bool is_float(const obj* o);
bool is_number(const obj* o);
bool is_t(const obj* o);

int get_int(const obj* o);
float get_float(const obj* o);

void dispose(obj* o);

#endif //_LISP_OBJECTS_H_INCLUDED
