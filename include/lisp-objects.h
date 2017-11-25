/*
 * File: lisp-objects.h
 * --------------------
 * Presents the interface of lisp object type definitions and creation of objects
 */

#ifndef _LISP_OBJECTS_H_INCLUDED
#define _LISP_OBJECTS_H_INCLUDED

#include <stdbool.h>

// The different types of lists
enum type {atom_obj, list_obj, primitive_obj, closure_obj, integer_obj, float_obj};

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
} closure_t;

typedef obj*(*primitive_t)(const obj*, obj**);

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
 * Function: new_primitive
 * -----------------------
 * Create a new primitive object
 * @param primitive: The primitive to wrap in an object
 * @return; The new primitive object wrapping the raw primitive instruction pointer
 */
obj* new_primitive(primitive_t primitive);

/**
 * Function: new_closure
 * ---------------------
 * Creates a new closure object
 * @return: A newly created closure object
 */
obj* new_closure();

/**
 * Function: get_list
 * ------------------
 * Gets a pointer to the list
 * @param o: Pointer to a lisp data structure
 * @return: A pointer to that list data structure's list
 */
list_t* list_of(const obj *o);

/**
 * Function: get_atom
 * ------------------
 * Gets a pointer to the atom
 * @param o: Pointer to a lisp data structure
 * @return: A pointer to the atom in the object
 */
atom_t atom_of(const obj *o);

/**
 * Function: get_primitive
 * -----------------------
 * Gets a pointer to a function pointer to the primitive function for a primitive object list
 * @param o: Pointer to a lisp data structure
 * @return: Function pointer to primitive function
 */
primitive_t* primitive_of(const obj *o);

/**
 * Function: closure_of
 * --------------------
 * Get the closure object stored in the given object
 * @param o: The object (should be a closure object, but this won't be checked)
 * @return: Pointer to the closure struct stored in the object
 */
closure_t* closure_of(const obj* o);

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
void* get_contents(const obj *o);

/**
 * Function: dispose
 * -----------------
 * Free the dynamically allocated memory used to store the lisp object.
 * @param o: Pointer to the lisp object to dispose of
 */
void dispose(obj* o);

#endif //_LISP_OBJECTS_H_INCLUDED
