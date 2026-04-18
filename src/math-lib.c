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

enum arith_op { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD };

static obj *apply_arithmetic(const obj *args, enum arith_op op, LispInterpreter *interpreter) {
  if (!CHECK_NARGS_MIN(args, 2)) return NULL;

  obj* result = eval(CAR(args), interpreter);
  if (result == NULL) return NULL;
  if (!is_number(result)) {
    LOG_ERROR("Argument is not a number.");
    return NULL;
  }

  for (const obj* rest = CDR(args); rest != NULL; rest = CDR(rest)) {
    obj* next = eval(CAR(rest), interpreter);
    if (next == NULL) return NULL;
    if (!is_number(next)) {
      LOG_ERROR("Argument is not a number.");
      return NULL;
    }

    obj *tmp;
    if (result->objtype == float_obj || next->objtype == float_obj) {
      float a = get_float(result), b = get_float(next);
      switch (op) {
        case OP_ADD: tmp = new_float(a + b); break;
        case OP_SUB: tmp = new_float(a - b); break;
        case OP_MUL: tmp = new_float(a * b); break;
        case OP_DIV: tmp = new_float(a / b); break;
        case OP_MOD: { float x = a > 0 ? a : -a, y = b > 0 ? b : -b;
                       while (x >= y) x -= y;
                       tmp = new_float(x); break; }
      }
    } else {
      int a = get_int(result), b = get_int(next);
      switch (op) {
        case OP_ADD: tmp = new_int(a + b); break;
        case OP_SUB: tmp = new_int(a - b); break;
        case OP_MUL: tmp = new_int(a * b); break;
        case OP_DIV: tmp = new_int(a / b); break;
        case OP_MOD: tmp = new_int(a % b); break;
      }
    }
    gc_add(&interpreter->gc, tmp);
    result = tmp;
  }

  return result;
}

def_primitive(add)    { return apply_arithmetic(args, OP_ADD, interpreter); }
def_primitive(sub)    { return apply_arithmetic(args, OP_SUB, interpreter); }
def_primitive(mul)    { return apply_arithmetic(args, OP_MUL, interpreter); }
def_primitive(divide) { return apply_arithmetic(args, OP_DIV, interpreter); }
def_primitive(mod) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  return apply_arithmetic(args, OP_MOD, interpreter);
}

enum cmp_op { CMP_EQ, CMP_GT, CMP_GTE, CMP_LT, CMP_LTE };

static obj *apply_compare(const obj *args, enum cmp_op op, LispInterpreter *interpreter) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  obj* first = eval(ith(args, 0), interpreter);
  if (first == NULL) return NULL;
  if (!is_number(first)) { LOG_ERROR("First argument is not a number."); return NULL; }
  obj* second = eval(ith(args, 1), interpreter);
  if (second == NULL) return NULL;
  if (!is_number(second)) { LOG_ERROR("Second argument is not a number."); return NULL; }
  bool result;
  if (is_int(first) && is_int(second)) {
    int a = get_int(first), b = get_int(second);
    switch (op) {
      case CMP_EQ:  result = a == b; break;
      case CMP_GT:  result = a > b; break;
      case CMP_GTE: result = a >= b; break;
      case CMP_LT:  result = a < b; break;
      case CMP_LTE: result = a <= b; break;
    }
  } else {
    float a = get_float(first), b = get_float(second);
    switch (op) {
      case CMP_EQ:  result = a == b; break;
      case CMP_GT:  result = a > b; break;
      case CMP_GTE: result = a >= b; break;
      case CMP_LT:  result = a < b; break;
      case CMP_LTE: result = a <= b; break;
    }
  }
  return result ? t(&interpreter->gc) : nil(&interpreter->gc);
}

def_primitive(equal) { return apply_compare(args, CMP_EQ, interpreter); }
def_primitive(gt)    { return apply_compare(args, CMP_GT, interpreter); }
def_primitive(gte)   { return apply_compare(args, CMP_GTE, interpreter); }
def_primitive(lt)    { return apply_compare(args, CMP_LT, interpreter); }
def_primitive(lte)   { return apply_compare(args, CMP_LTE, interpreter); }
