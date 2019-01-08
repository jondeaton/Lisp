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
#include <primitives.h>

obj *eval(const obj *o, struct LispInterpreter *interpreter) {
  if (o == NULL) return NULL;

  // Truth, nil, numbers, primitives and closures evaluate to themselves
  if (is_t(o) || is_nil(o) || is_number(o) || is_primitive(o) || is_closure(o))
    return (obj*) o;

  // Atom type means its just a literal that needs to be looked up
  if (is_atom(o)) {
    obj* value = env_lookup(&interpreter->env, o);
    if (value == NULL) {
      LOG_ERROR("Variable: \"%s\" not found in environment", ATOM(o));
      return NULL;
    }
    return value;
  }

  // List type means its a operator being applied to operands which means evaluate
  // the operator (return a procedure or a primitive) to which we call apply on the arguments
  if (is_list(o)) {
    obj* oper = eval(CAR(o), interpreter);
    return apply(oper, CDR(o), interpreter);
  }
  LOG_ERROR("Object of unknown type");
  return NULL;
}

obj *apply(const obj *oper, const obj *args, struct LispInterpreter *interpreter) {
  if (oper == NULL) return NULL;

  if (is_primitive(oper)) {

    primitive_t f = *PRIMITIVE(oper);
    return f(args, interpreter);
  }

  if (is_closure(oper)) {
    if (!CHECK_NARGS_MAX(args, NARGS(oper))) return NULL;

    // Partial closure application
    if (list_length(args) < NARGS(oper))
      return closure_partial_application(oper, args, interpreter);

    // The result of a closure application is the evaluation of the body of the closure in an environment
    // containing the captured variables from the closure, along with the values of the arguments
    // bound to the parameters of the closure.

    push_scope(&interpreter->env);
    bind_closure(&interpreter->env, oper, args);

    // Evaluate body in new environment
    obj* result = eval(PROCEDURE(oper), interpreter);

    pop_scope(&interpreter->env);

    return result;
  }

  if (is_atom(oper)) {
    LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(oper));
    return NULL;
  }
  LOG_ERROR("Non-procedure cannot be applied");
  return NULL;
}
