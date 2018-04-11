/**
 * @file math.c
 * @brief Implementation of the math library
 * @details Full disclosure, this file is honestly not that great.
 */

#include <math.h>
#include <garbage-collector.h>
#include <parser.h>
#include <list.h>
#include "environment.h"
#include <evaluator.h>
#include <stack-trace.h>
#include <lisp-objects.h>

// function typedefs for Z^2 --> Z, and R^2 --> R
typedef int (*intArithmeticFuncPtr)(int, int);
typedef float (*floatArithmeticFuncPtr)(float, float);

// Static declarations
static obj *apply_arithmetic(const obj *args, obj **env,
                             intArithmeticFuncPtr int_op,
                             floatArithmeticFuncPtr float_op,
                             GarbageCollector *gc);

// List of reserved words in the lisp language
static atom_t const math_reserved_atoms[] = { "+", "-", "*", "/", "%",
                                              "=", ">", ">=", "<", "<=", NULL };

// List of the primitives associated with each of the reserved words
static const primitive_t math_primitives[]= {&add, &sub, &mul, &divide, &mod,
                                             &equal, &gt, &gte, &lt, &lte, NULL };

obj* get_math_library() {
  return create_environment(math_reserved_atoms, math_primitives);
}

/**
 * The following preprocessor definitions allow for the easy creation of operators
 */
#define DEF_OP(name, T, op) static inline T name ## _ ## T ##s (T x, T y) { return x op y; }
#define DEF_OPS(name, op) DEF_OP(name, int, op) DEF_OP(name, float, op)
DEF_OPS(add, +)
DEF_OPS(sub, -)
DEF_OPS(mul, *)
DEF_OPS(divide, /)
DEF_OP(mod, int, %)
static float mod_floats(float x, float y) { // This one kinda needs it's own special definition
  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;
  while (x >= y) x -= y;
  return x;
}

// I really don't like typing things
#define DEF_ALLOC(T) static obj* allocate_ ## T (T value, GarbageCollector* gc) {\
  obj* o = new_int(value); \
  gc_add(gc, o); \
  return o; }
DEF_ALLOC(int)
DEF_ALLOC(float)

// I super hate typing things
#define DEF_OPERATOR(name) obj* name(const obj *args, obj **envp, GarbageCollector *gc) { \
  return apply_arithmetic(args, envp, &(name ## _ints), &(name ## _floats), gc); \
}
DEF_OPERATOR(add)
DEF_OPERATOR(sub)
DEF_OPERATOR(mul)
DEF_OPERATOR(divide)
DEF_OPERATOR(mod)

#define DEF_CMP(name, op) def_primitive(name) { \
  if (!CHECK_NARGS(args, 2)) return NULL; \
  obj* first = eval(ith(args, 0), envp, gc); \
  obj* second = eval(ith(args, 1), envp, gc); \
  if (!is_number(first) || !is_number(second)) return empty(gc); \
  if (is_int(first) && is_int(second)) \
    return get_int(first) op get_int(second) ? t(gc) : empty(gc); \
  return get_float(first) op get_float(second) ? t(gc) : empty(gc); \
}
DEF_CMP(equal, ==)
DEF_CMP(gt, >)
DEF_CMP(gte, >=)
DEF_CMP(lt, <)
DEF_CMP(lte, <=)

/**
 * Function: apply_arithmetic
 * --------------------------
 * Apply an arithmetic operation
 * @param args: The lisp object containing the argument list
 * @param env: The environment to apply the operation in
 * @param int_op: The operation corresponding to applying to integers
 * @param float_op: The operation corresponding to applying to floating point numbers
 * @return: The result of applying the arithmetic operation to the values of the arguments
 */
static obj *apply_arithmetic(const obj *args, obj **env,
                             intArithmeticFuncPtr int_op,
                             floatArithmeticFuncPtr float_op,
                             GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), env, gc);
  if (!first) return NULL;
  obj* second = eval(ith(args, 1), env, gc);
  if (!second) return NULL;

  if (first->objtype == float_obj || second->objtype == float_obj ) {
    float value = float_op(get_float(first), get_float(second));
    return allocate_float(value, gc);
  } else {
    int value = int_op(get_int(first), get_int(second));
    return allocate_int(value, gc);
  }
}

