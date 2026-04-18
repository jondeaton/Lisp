/*
 * File: math-lib.c
 * ----------------
 * Implementation of the math library
 */

#include <math-lib.h>
#include <interpreter.h>
#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>

static atom_t const math_reserved_atoms[] = { "+", "-", "*", "/", "%",
                                              "=", ">", ">=", "<", "<=", NULL };

static const primitive_t math_primitives[]= { &add, &sub, &mul, &divide, &mod,
                                              &equal, &gt, &gte, &lt, &lte, NULL };

obj* get_math_library(void) {
  return create_environment(math_reserved_atoms, math_primitives);
}

static obj *apply_arithmetic(const obj *args, obj *(*int_op)(int, int),
                             obj *(*float_op)(float, float), LispInterpreter *interpreter);

static obj *add_ints(int x, int y)       { obj *o = new_int(x + y); return o; }
static obj *sub_ints(int x, int y)       { obj *o = new_int(x - y); return o; }
static obj *mul_ints(int x, int y)       { obj *o = new_int(x * y); return o; }
static obj *divide_ints(int x, int y)    { obj *o = new_int(x / y); return o; }
static obj *mod_ints(int x, int y)       { obj *o = new_int(x % y); return o; }

static obj *add_floats(float x, float y)    { return new_float(x + y); }
static obj *sub_floats(float x, float y)    { return new_float(x - y); }
static obj *mul_floats(float x, float y)    { return new_float(x * y); }
static obj *divide_floats(float x, float y) { return new_float(x / y); }
static obj *mod_floats(float x, float y) {
  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;
  while (x >= y) x -= y;
  return new_float(x);
}

def_primitive(add)    { return apply_arithmetic(args, add_ints, add_floats, interpreter); }
def_primitive(sub)    { return apply_arithmetic(args, sub_ints, sub_floats, interpreter); }
def_primitive(mul)    { return apply_arithmetic(args, mul_ints, mul_floats, interpreter); }
def_primitive(divide) { return apply_arithmetic(args, divide_ints, divide_floats, interpreter); }
def_primitive(mod)    { return apply_arithmetic(args, mod_ints, mod_floats, interpreter); }

#define def_math_compare(name, op) def_primitive(name) { \
  if (!CHECK_NARGS(args, 2)) return NULL; \
  obj* first = eval(ith(args, 0), interpreter); \
  if (first == NULL) return NULL; \
  if (!is_number(first)) { LOG_ERROR("First argument is not a number."); return NULL; } \
  obj* second = eval(ith(args, 1), interpreter); \
  if (second == NULL) return NULL; \
  if (!is_number(second)) { LOG_ERROR("Second argument is not a number."); return NULL; } \
  if (is_int(first) && is_int(second)) \
    return get_int(first) op get_int(second) ? t(&interpreter->gc) : nil(&interpreter->gc); \
  return get_float(first) op get_float(second) ? t(&interpreter->gc) : nil(&interpreter->gc); \
}
def_math_compare(equal, ==)
def_math_compare(gt, >)
def_math_compare(gte, >=)
def_math_compare(lt, <)
def_math_compare(lte, <=)

static obj *apply_arithmetic(const obj *args, obj *(*int_op)(int, int),
                             obj *(*float_op)(float, float), LispInterpreter *interpreter) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), interpreter);
  if (first == NULL) return NULL;
  if (!is_number(first)) {
    LOG_ERROR("First argument is not a number.");
    return NULL;
  }

  obj* second = eval(ith(args, 1), interpreter);
  if (second == NULL) return NULL;
  if (!is_number(second)) {
    LOG_ERROR("Second argument is not a number.");
    return NULL;
  }

  obj *result;
  if (first->objtype == float_obj || second->objtype == float_obj) {
    result = float_op(get_float(first), get_float(second));
  } else {
    result = int_op(get_int(first), get_int(second));
  }
  gc_add(&interpreter->gc, result);
  return result;
}
