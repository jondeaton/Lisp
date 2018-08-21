/*
 * File: math.h
 * ------------
 * Presents the interface of the math library
 */

#ifndef _LISP_MATH_H_INCLUDED
#define _LISP_MATH_H_INCLUDED

#include "list.h"
#include "memory-manager.h"

/**
 * Function: get_math_library
 * --------------------------
 * Get the math library environment
 * @return: The math library environment
 */
obj* get_math_library();

/**
 * Primitive: add
 * ---------------
 * Primitive function for adding two numbers
 * @param args: The unevaluated arguments to add
 * @param envp: The environment to evaluate the addition
 * @return: A newly allocated number/float object containing the result of the addition
 */
obj *add(const obj *args, obj **envp, MemoryManager *gc);

/**
 * Primitive: sub
 * -------------------
 * Primitive function for subtracting two numbers
 * @param o: The unevaluated arguments to subtract
 * @param env: The environment to evaluate the subtraction
 * @return: A newly allocated number/float object containing the result of the subtraction
 */
obj *sub(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: mul
 * -------------------
 * Primitive function for multiplying two numbers
 * @param o: The unevaluated arguments to multiply
 * @param env: The environment to evaluate the multiplication
 * @return: A newly allocated number/float object containing the result of the multiplication
 */
obj *mul(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: divide
 * -----------------
 * Primitive function for dividing two numbers
 * @param o: The unevaluated arguments to divide
 * @param env: The environment to evaluate the division
 * @return: A newly allocated number/float object containing the result of the division
 */
obj *divide(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: mod
 * --------------
 * Primitive function for taking modulus of two numbers
 * @param o: The unevaluated arguments to use to take the modulus
 * @param env: The environment to evaluate the modulus
 * @return: A newly allocated number/float object containing the result of the modulus
 */
obj *mod(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: equal
 * ----------------
 *  Test Equality
 * @param o
 * @param env
 * @param gc
 * @return
 */
obj *equal(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: gt
 * -------------
 * Greater than
 * @param o
 * @param env
 * @param gc
 * @return
 */
obj *gt(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: gte
 * --------------
 * Greater than or equals
 * @param o
 * @param env
 * @param gc
 * @return
 */
obj *gte(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: lt
 * -------------
 * Less than
 * @param o
 * @param env
 * @param gc
 * @return
 */
obj *lt(const obj *o, obj **env, MemoryManager *gc);

/**
 * Primitive: lte
 * --------------
 * Less than or equal to
 * @param o
 * @param env
 * @param gc
 * @return
 */
obj *lte(const obj *o, obj **env, MemoryManager *gc);

#endif // _LISP_MATH_H_INCLUDED
