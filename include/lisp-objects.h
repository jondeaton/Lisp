/*
 * File: lisp-objects.h
 * --------------------
 * Presents the interface of lisp object type definitions and creation of objects
 */

#ifndef _LISP_OBJECTS_H_INCLUDED
#define _LISP_OBJECTS_H_INCLUDED

#include <stdbool.h>

// The different types of lists
enum type {
    atom_obj,
    list_obj,
    primitive_obj,
    closure_obj,
    int_obj,
    float_obj
};

typedef const char* atom_t;

typedef struct {
  enum type objtype;
} obj;

typedef struct {
  obj* car;
  obj* cdr;
} list_t;

typedef struct {
  obj* parameters;
  obj* procedure;
  obj* captured;
  int nargs;
} closure_t;

#define CONTENTS(o)   ((void*) ((char*) (o) + sizeof(obj)))
#define ATOM(o)       ((atom_t)  CONTENTS(o))
#define LIST(o)       ((list_t*) CONTENTS(o))
#define PRIMITIVE(o)  ((primitive_t*) CONTENTS(o))
#define CLOSURE(o)    ((closure_t*)   CONTENTS(o))

// Useful for extracting elements from the lisp object
#define CAR(o) LIST(o)->car
#define CDR(o) LIST(o)->cdr
#define PARAMETERS(o) CLOSURE(o)->parameters
#define PROCEDURE(o)  CLOSURE(o)->procedure
#define CAPTURED(o)   CLOSURE(o)->captured
#define NARGS(o)      CLOSURE(o)->nargs

/**
 * Function: new_atom
 * ------------------
 * Create a new atom object
 * @param name: The name to store in the atom object
 * @return: The new atom object wrapping the raw atom
 */
obj* new_atom(atom_t name);

/**
 * Function: new_list
 * ------------------
 * Returns a list object in dynamically allocated memory
 * @return: A pointer to a new list object in dynamically allocated memory
 */
obj* new_list();

/**
 * Function: new_closure
 * ---------------------
 * Creates a new closure object
 * @return: A newly created closure object
 */
obj* new_closure();

/**
 * Function: new_int
 * -----------------
 * Creates a new integer object wrapping a raw integer value.
 * Note: This object will not be automatically added to the "allocated" list
 * @param value: The integer value to wrap in an object
 * @return: The object wrapping a copy of the integer value in dynamically allocated memory
 */
obj* new_int(int value);

/**
 * Function: new_float
 * -------------------
 * Creates a new float object wrapping a raw integer value.
 * Note: This object will not be automatically added to the "allocated" list
 * @param value: The float value to wrap in an object
 * @return: The object wrapping a copy of the floating point value
 */
obj* new_float(float value);

/**
 * Function: copy_atom
 * -------------------
 * Copy an object that is an atom by dynamically allocating space for an identical object, and then
 * copying the contents of the atom over into the new object.
 * @param o: The object (of type atom) to copy
 * @return: A pointer to a copy of the object in dynamically allocated space
 */
obj* copy_atom(const obj* o);

/**
 * Function: copy_list
 * -------------------
 * Copy a list object NOT recursively
 * @param o: The list object to copy
 * @return: A copy of the list in dynamically allocated memory
 */
obj* copy_list(const obj *o);

/**
 * Function: compare
 * -----------------
 * Compares two lisp objects in a non-recursive way
 * @param a: The first lisp object
 * @param b: The second lisp object
 * @return: True if both objects are the same, false otherwise
 */
bool compare(const obj* a, const obj* b);

/**
 * Function: is_atom
 * -----------------
 * Determines if an object is of the atom type
 * @param o: The object to check if it is an atom
 * @return: True if the object type is atom, false otherwise
 */
bool is_atom(const obj* o);

/**
 * Function: is_primitive
 * ----------------------
 * Determines if an object is of the primitive type
 * @param o: The object to check whether it is primitive
 * @return: True if the object type is a primitive, false otherwise
 */
bool is_primitive(const obj* o);

/**
 * Function: is_list
 * -----------------
 * Determines if an object is of the list type
 * @param o: The object to check whether it is list
 * @return: True if the object type is a list, false otherwise
 */
bool is_list(const obj* o);

/**
 * Function: is_closure
 * ----------------------
 * Determines if an object is of the closure type
 * @param o: The object to check whether it is closure
 * @return: True if the object type is a closure, false otherwise
 */
bool is_closure(const obj* o);

/**
 * Function: is_int
 * ----------------
 * Determines if an object is of the integer type
 * @param o: The object to check whether it is an integer
 * @return: True if the object type is an integer, false otherwise
 */
bool is_int(const obj* o);

/**
 * Function: is_float
 * ------------------
 * Determines if an object is of the float type
 * @param o: The object to check whether it is a float
 * @return: True if the object type is a float, false otherwise
 */
bool is_float(const obj* o);

/**
 * Function: is_number
 * -------------------
 * Determines if an object is a number type
 * @param o: The object to check whether it is a number
 * @return: True if the object type is a int or float, false otherwise
 */
bool is_number(const obj* o);

/**
 * Function: is_t
 * --------------
 * Determines if a lisp object is the truth atom
 * @param o: A lisp object to determine if it is the truth atom
 * @return: True if it is the truth atom, false otherwise.
 */
bool is_t(const obj* o);

/**
 * Function: get_int
 * -----------------
 * Get the integer value from an object wrapping a number
 * @param o: The object that is wrapping the integer or the float
 * @return: A copy of the value stored in the object as an integer
 */
int get_int(const obj* o);

/**
 * Function: get_float
 * -------------------
 * Get the floating point value from an object wrapping a number
 * @param o: The object wrapping the integer or floating point value
 * @return: A copy of the value stored in the object as a floating point
 */
float get_float(const obj* o);

/**
 * Function: get_contents
 * ----------------------
 * Utility function for getting the contents of the object that exists just to the right of
 * the object type enum.
 * @param o: Object to get the contents of
 * @return: Pointer to the memory containing the contents of the object
 */
//void* CONTENTS(const obj *o);

/**
 * Function: dispose
 * -----------------
 * Free the dynamically allocated memory used to store the lisp object.
 * @param o: Pointer to the lisp object to dispose of
 */
void dispose(obj* o);

#endif //_LISP_OBJECTS_H_INCLUDED
