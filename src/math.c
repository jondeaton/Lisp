/*
 * File: math.c
 * ------------
 * Presents the implementation of the math library
 */

#include <parser.h>
#include <list.h>
#include "environment.h"
#include "math.h"
#include "evaluator.h"
#include "stack-trace.h"

typedef int (*intArithmeticFuncPtr)(int, int);
typedef float (*floatArithmeticFuncPtr)(float, float);

static obj* allocate_integer(int value);
static obj* allocate_float(float value);
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
static obj* do_arithmetic(const obj* o, obj* env, intArithmeticFuncPtr intOp, floatArithmeticFuncPtr floatOp);

static atom_t const math_reserved_atoms[] = { "+", "-", "*", "/", "%", NULL };
static const primitive_t math_primitives[]= { &plus, &subtract, &multiply, &divide, &mod, NULL };

obj* get_math_library() {
  return make_environment(math_reserved_atoms, math_primitives);
}

obj* plus(const obj* o, obj* env) {
  obj* answer = do_arithmetic(o, env, &add_ints, &add_floats);
  if (answer == NULL) log_error(__func__, "Plus operation returned in NULL");
  return answer;
}

obj* subtract(const obj* o, obj* env) {
  obj* answer = do_arithmetic(o, env, &sub_ints, &sub_floats);
  if (answer == NULL) log_error(__func__, "Subtract operation returned in NULL");
  return answer;
}

obj* multiply(const obj* o, obj* env) {
  obj* answer = do_arithmetic(o, env, &mul_ints, &mul_floats);
  if (answer == NULL) log_error(__func__, "Multiply operation returned in NULL");
  return answer;
}

obj* divide(const obj* o, obj* env) {
  obj* answer = do_arithmetic(o, env, &div_ints, &div_floats);
  if (answer == NULL) log_error(__func__, "Divide operation returned in NULL");
  return answer;
}

obj* mod(const obj* o, obj* env) {
  obj* answer = do_arithmetic(o, env, &mod_ints, &mod_floats);
  if (answer == NULL) log_error(__func__, "Mod operation returned in NULL");
  return answer;
}

// todo: Add documentation

static obj* do_arithmetic(const obj* o, obj* env, intArithmeticFuncPtr intOp, floatArithmeticFuncPtr floatOp) {
  obj* first_arg = ith(o, 0);
  obj* second_arg = ith(o, 1);

  if (first_arg == NULL) {
    log_error(__func__, "First argument not found");
    return NULL;
  }

  if (second_arg == NULL) {
    log_error(__func__, "Second argument not found");
    return NULL;
  }

  if (ith(o, 2) != NULL) {
    log_error(__func__, "Too many arguments");
    return NULL;
  }

  obj* first = eval(first_arg, env);
  obj* second = eval(second_arg, env);

  if (first == NULL) {
    log_error(__func__, "First argument evaluated to NULL");
    return NULL;
  }

  if (second == NULL) {
    log_error(__func__, "Second argument evaluated to NULL");
    return NULL;
  }

  if (first->objtype == float_obj || second->objtype == float_obj ) {
    float value = floatOp(get_float(first), get_float(second));
    return allocate_float(value);
  } else {
    int value = intOp(get_int(first), get_int(second));
    return allocate_integer(value);
  }
}

static obj* allocate_integer(int value) {
  obj* o = new_int(value);
  add_allocated(o);
  return o;
}

static obj* allocate_float(float value) {
  obj* o = new_float(value);
  add_allocated(o);
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
