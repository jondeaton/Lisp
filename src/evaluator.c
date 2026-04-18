/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include <list.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>
#include <string.h>
#include <lisp-objects.h>
#include <closure.h>
#include <garbage-collector.h>
#include <interpreter.h>

obj *eval(const obj *o, LispInterpreter *interpreter) {
  if (o == NULL) return NULL;

  // Atom type means its just a literal that needs to be looked up
  if (is_atom(o)) {
    if (is_t(o)) return (obj*) o;
    obj* value = lookup(o, interpreter->env);
    if (value == NULL) {
      LOG_ERROR("Variable: \"%s\" not found in environment", ATOM(o));
      return NULL;
    }
    return value;
  }

  // Numbers, primitives, closures and macros evaluate to themselves
  if (is_number(o) || is_primitive(o) || is_closure(o) || is_macro(o)) return (obj*) o;

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (is_list(o)) {
    if (is_nil(o)) return (obj*) o;                     // Empty list evaluates to itself

    obj* oper = eval(CAR(o), interpreter);
    return apply(oper, CDR(o), interpreter);
  }
  LOG_ERROR("Object of unknown type");
  return NULL;
}

obj *apply(const obj *oper, const obj *args, LispInterpreter *interpreter) {
  if (oper == NULL) return NULL;

  if (is_primitive(oper)) {
    primitive_t f = PRIMITIVE(oper);
    return f(args, interpreter);
  }

  if (is_closure(oper)) {
    if (NARGS(oper) >= 0) {
      if (!CHECK_NARGS_MAX(args, NARGS(oper))) return NULL;
      if (list_length(args) < NARGS(oper))
        return closure_partial_application(oper, args, interpreter);
    }

    // Lexical scoping: evaluate args in caller's env, then bind params
    // in the closure's definition-time env (CAPTURED)
    obj* frame = associate(PARAMETERS(oper), args, interpreter, true);
    obj* new_env = join_lists(frame, CAPTURED(oper));

    obj* old_env = interpreter->env;
    interpreter->env = new_env;
    obj* result = eval(PROCEDURE(oper), interpreter);
    interpreter->env = old_env;

    return result;
  }

  if (is_macro(oper)) {
    if (NARGS(oper) >= 0 && !CHECK_NARGS(args, NARGS(oper))) return NULL;

    // Macros: bind raw (unevaluated) args to params, eval body to get expansion,
    // then eval expansion in the caller's env
    obj* frame = associate(PARAMETERS(oper), args, interpreter, false);
    obj* macro_env = join_lists(frame, CAPTURED(oper));

    obj* old_env = interpreter->env;
    interpreter->env = macro_env;
    obj* expansion = eval(PROCEDURE(oper), interpreter);
    interpreter->env = old_env;

    if (expansion == NULL) return NULL;
    return eval(expansion, interpreter);
  }

  if (is_atom(oper)) {
    LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(oper));
    return NULL;
  }
  LOG_ERROR("Non-procedure cannot be applied");
  return NULL;
}
