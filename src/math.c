/*
 * File: math.c
 * ------------
 * Presents the implementation of the math library
 */

#include <math.h>
#include <garbage-collector.h>
#include <parser.h>
#include <list.h>
#include "environment.h"
#include <evaluator.h>
#include <stack-trace.h>
#include <lisp-objects.h>

typedef int (*intArithmeticFuncPtr)(int, int);
typedef float (*floatArithmeticFuncPtr)(float, float);

static obj *allocate_integer(int value, GarbageCollector *gc);
static obj *allocate_float(float value, GarbageCollector *gc);
static int add_ints(int x, int y);
static float add_floats(float x, float y);
static int sub_ints(int x, int y);
static float sub_floats(float x, float y);
static int mul_ints(int x, int y);
static float mul_floats(float x, float y);
static int div_ints(int x, int y);
static float div_floats(float x, float y);
static int mod_ints(int x, int y);
static float mod_floats(float x, float y);
obj *equal(const obj *args, obj **envp, GarbageCollector *gc);
static obj *apply_arithmetic(const obj *args, obj **env, intArithmeticFuncPtr intOp, floatArithmeticFuncPtr floatOp,
                            GarbageCollector *gc);

static atom_t const math_reserved_atoms[] = { "+", "-", "*", "/", "%", "=", NULL };
static const primitive_t math_primitives[]= {&plus, &subtract, &multiply, &divide, &mod, &equal, NULL };

obj* get_math_library() {
  return make_environment(math_reserved_atoms, math_primitives);
}

obj *plus(const obj *args, obj **envp, GarbageCollector *gc) {
  obj* answer = apply_arithmetic(args, envp, &add_ints, &add_floats, gc);
  if (answer == NULL) LOG_ERROR("Addition operator returned in NULL");
  return answer;
}

obj *subtract(const obj *o, obj **env, GarbageCollector *gc) {
  obj* answer = apply_arithmetic(o, env, &sub_ints, &sub_floats, gc);
  if (answer == NULL) LOG_ERROR("Subtraction operator returned in NULL");
  return answer;
}

obj *multiply(const obj *o, obj **env, GarbageCollector *gc) {
  obj* answer = apply_arithmetic(o, env, &mul_ints, &mul_floats, gc);
  if (answer == NULL) LOG_ERROR("Multiplication operator returned in NULL");
  return answer;
}

obj *divide(const obj *o, obj **env, GarbageCollector *gc) {
  obj* answer = apply_arithmetic(o, env, &div_ints, &div_floats, gc);
  if (answer == NULL) LOG_ERROR("Division operator returned in NULL");
  return answer;
}

obj *mod(const obj *o, obj **env, GarbageCollector *gc) {
  obj* answer = apply_arithmetic(o, env, &mod_ints, &mod_floats, gc);
  if (answer == NULL) LOG_ERROR("Modulus operator returned in NULL");
  return answer;
}

obj *equal(const obj *args, obj **envp, GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  obj* first = eval(ith(args, 0), envp, gc);
  obj* second = eval(ith(args, 1), envp, gc);
  if (!is_number(first) || !is_number(second)) return empty(gc);
  if (is_int(first) && is_int(second))
    return get_int(first) == get_int(second) ? t(gc) : empty(gc);
  return get_float(first) == get_float(second) ? t(gc) : empty(gc);
}

/**
 * Function: apply_arithmetic
 * --------------------------
 * Apply an arithmetic operation
 * @param args: The lisp object containing the argument list
 * @param env: The environment to apply the operation in
 * @param intOp: The operation corresponding to applying to integers
 * @param floatOp: The operation corresponding to applying to floating point numbers
 * @return: The result of applying the arithmetic operation to the values of the arguments
 */
static obj *apply_arithmetic(const obj *args, obj **env,
                             intArithmeticFuncPtr intOp, floatArithmeticFuncPtr floatOp,
                             GarbageCollector *gc) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), env, gc);
  obj* second = eval(ith(args, 1), env, gc);

  if (first == NULL) return LOG_ERROR("First argument evaluated to NULL");
  if (second == NULL) return LOG_ERROR("Second argument evaluated to NULL");

  if (first->objtype == float_obj || second->objtype == float_obj ) {
    float value = floatOp(get_float(first), get_float(second));
    return allocate_float(value, gc);
  } else {
    int value = intOp(get_int(first), get_int(second));
    return allocate_integer(value, gc);
  }
}

static obj *allocate_integer(int value, GarbageCollector *gc) {
  obj* o = new_int(value);
  gc_add(gc, o);
  return o;
}

static obj *allocate_float(float value, GarbageCollector *gc) {
  obj* o = new_float(value);
  gc_add(gc, o);
  return o;
}

static int add_ints(int x, int y) {
  return x + y;
}

static float add_floats(float x, float y) {
  return x + y;
}

static int sub_ints(int x, int y) {
  return x - y;
}

static float sub_floats(float x, float y) {
  return x - y;
}

static int mul_ints(int x, int y) {
  return x * y;
}

static float mul_floats(float x, float y) {
  return x * y;
}

static int div_ints(int x, int y) {
  return x / y;
}

static float div_floats(float x, float y) {
  return x / y;
}

static int mod_ints(int x, int y) {
  return x % y;
}

static float mod_floats(float x, float y) {
  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;
  while (x >= y) x -= y;
  return x;
}
