/**
 * @file math.c
 * @brief Implementation of the math library
 * @details Full disclosure, this file is honestly not that great.
 */

#include <math-lib.h>
#include <interpreter.h>
#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>


// function typedefs for Z^2 --> Z, and R^2 --> R
typedef int (*intArithmeticFuncPtr)(int, int);
typedef float (*floatArithmeticFuncPtr)(float, float);

// Static declarations
static obj *apply_arithmetic(const obj *args, intArithmeticFuncPtr int_op,
                             floatArithmeticFuncPtr float_op, struct LispInterpreter *interpreter);

// List of reserved words in the lisp language
static atom_t const math_reserved_atoms[] = { "+", "-", "*", "/", "%",
                                        "=", ">", ">=", "<", "<=", NULL };

// List of the primitives associated with each of the reserved words
static const primitive_t math_primitives[]= { &add, &sub, &mul, &divide, &mod,
                                              &equal, &gt, &gte, &lt, &lte, NULL };

obj* get_math_library() {
  // todo: whatever this should be
  //  return create_environment(math_reserved_atoms, math_primitives);
  return NULL;
}

// Define basic functions for arithmetic operations on two numbers
#define def_single_op(name, T, op) static inline T name ## _ ## T ##s (T x, T y) { return x op y; }
#define def_two_ops(name, op) def_single_op(name, int, op) def_single_op(name, float, op)
def_two_ops(add, +)
def_two_ops(sub, -)
def_two_ops(mul, *)
def_two_ops(divide, /)
def_single_op(mod, int, %)
static float mod_floats(float x, float y) { // This one needs it's own special definition
  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;
  while (x >= y) x -= y;
  return x;
}

// Function definitions for integer and floating point allocators
#define def_number_allocator(T) static obj* allocate_ ## T (T value, struct GarbageCollector* gc) {\
  obj* o = new_int(value); \
  gc_add(gc, o); \
  return o; }

def_number_allocator(int)

def_number_allocator(float)

// macro for defining a the primitive operator
#define def_math_op_primitive(name) def_primitive(name) { \
  return apply_arithmetic(args, &(name ## _ints), &(name ## _floats), interpreter); \
}
def_math_op_primitive(add)
def_math_op_primitive(sub)
def_math_op_primitive(mul)
def_math_op_primitive(divide)
def_math_op_primitive(mod)

// Define a mathematical comparison primitive
#define def_math_compare_primitive(name, op) def_primitive(name) { \
  if (!CHECK_NARGS(args, 2)) return NULL; \
  obj* first = eval(ith(args, 0), interpreter); \
  if (first == NULL) return NULL; \
  if (!is_number(first)) { \
      LOG_ERROR("First argument did not evaluate to a number."); \
      return NULL; \
  } \
  obj* second = eval(ith(args, 1), interpreter); \
  if (second == NULL) return NULL; \
  if (!is_number(second)) { \
    LOG_ERROR("Second argument did not evaluate to a number."); \
    return NULL; \
  } \
  if (is_int(first) && is_int(second)) \
    return get_int(first) op get_int(second) ? t(&interpreter->gc) : nil(&interpreter->gc); \
  return get_float(first) op get_float(second) ? t(&interpreter->gc) : nil(&interpreter->gc); \
}
def_math_compare_primitive(equal, ==)
def_math_compare_primitive(gt, >)
def_math_compare_primitive(gte, >=)
def_math_compare_primitive(lt, <)
def_math_compare_primitive(lte, <=)

/**
 * Function: apply_arithmetic
 * --------------------------
 * Apply an arithmetic operation, the core of all operators
 * @param args: The lisp object containing the argument list
 * @param envp: The environment to apply the operation in
 * @param int_op: The operation corresponding to applying to integers
 * @param float_op: The operation corresponding to applying to floating point numbers
 * @return: The result of applying the arithmetic operation to the values of the arguments
 */
static obj *apply_arithmetic(const obj *args, intArithmeticFuncPtr int_op,
                             floatArithmeticFuncPtr float_op, struct LispInterpreter *interpreter) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), interpreter);
  if (first == NULL) return NULL;
  if (!is_number(first)) {
    LOG_ERROR("First argument did not evaluate to a number.");
    return NULL;
  }

  obj* second = eval(ith(args, 1), interpreter);
  if (second == NULL) return NULL;
  if (!is_number(second)) {
    LOG_ERROR("Second argument did not evaluate to a number.");
    return NULL;
  }

  if (first->objtype == float_obj || second->objtype == float_obj ) {
    float value = float_op(get_float(first), get_float(second));
    return allocate_float(value, &interpreter->gc);
  } else {
    int value = int_op(get_int(first), get_int(second));
    return allocate_int(value, &interpreter->gc);
  }
}
