/*
 * File: math.h
 * ------------
 * Presents the interface of the math library
 */

#include "list.h"

/**
 * Function: add_math_library
 * --------------------------
 * Get the math library environment
 * @return: The math library environment
 */
obj* get_math_library();

/**
 * Primitive: plus
 * ---------------
 * Primitive function for adding two numbers
 * @param o: The unevaluated arguments to add
 * @param env: The environment to evaluate the addition
 * @return: A newly allocated number/float object containing the result of the addition
 */
obj* plus(const obj* o, obj* env);

/**
 * Primitive: subtract
 * -------------------
 * Primitive function for subtracting two numbers
 * @param o: The unevaluated arguments to subtract
 * @param env: The environment to evaluate the subtraction
 * @return: A newly allocated number/float object containing the result of the subtraction
 */
obj* subtract(const obj* o, obj* env);

/**
 * Primitive: multiply
 * -------------------
 * Primitive function for multiplying two numbers
 * @param o: The unevaluated arguments to multiply
 * @param env: The environment to evaluate the multiplication
 * @return: A newly allocated number/float object containing the result of the multiplication
 */
obj* multiply(const obj* o, obj* env);

/**
 * Primitive: divide
 * -----------------
 * Primitive function for dividing two numbers
 * @param o: The unevaluated arguments to divide
 * @param env: The environment to evaluate the division
 * @return: A newly allocated number/float object containing the result of the division
 */
obj* divide(const obj* o, obj* env);

/**
 * Primitive: mod
 * --------------
 * Primitive function for taking modulus of two numbers
 * @param o: The unevaluated arguments to use to take the modulus
 * @param env: The environment to evaluate the modulus
 * @return: A newly allocated number/float object containing the result of the modulus
 */
obj* mod(const obj* o, obj* env);