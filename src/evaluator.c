/*
 * File: evaluator.c
 * -----------------
 * Presents the implementation of the lisp evaluator
 */

#include <list.h>
#include <evaluator.h>
#include <environment.h>
#include <primitives.h>
#include <stack-trace.h>
#include <string.h>
#include <lisp-objects.h>
#include <closure.h>
#include <garbage-collector.h>
#include <interpreter.h>

obj *eval(const obj *o, LispInterpreter *interpreter) {
  obj *caller_env = NULL;  // saved only on first closure tail call
  obj *result = NULL;

  while (true) {
    if (o == NULL) break;

    if (is_atom(o)) {
      if (is_t(o)) { result = (obj*)o; break; }
      result = lookup(o, interpreter->env);
      if (result == NULL)
        LOG_ERROR("Variable: \"%s\" not found in environment", ATOM(o));
      break;
    }

    if (is_number(o) || is_string(o) || is_vector(o) || is_primitive(o) || is_closure(o) || is_macro(o)) {
      result = (obj*)o;
      break;
    }

    if (!is_list(o)) {
      LOG_ERROR("Object of unknown type");
      break;
    }

    if (is_nil(o)) { result = (obj*)o; break; }

    // Special form: cond — handled inline for tail-call optimization
    if (is_atom(CAR(o)) && strcmp(ATOM(CAR(o)), "cond") == 0) {
      const obj *clauses = CDR(o);
      bool tail_call = false;
      bool error = false;

      for (const obj *c = clauses; c != NULL && is_list(c) && !is_nil(c); c = CDR(c)) {
        obj *clause = CAR(c);
        if (!is_list(clause) || is_nil(clause)) {
          if (is_nil(clause)) LOG_ERROR("Empty Conditional pair.");
          else LOG_ERROR("Conditional pair clause is not a list");
          error = true;
          break;
        }
        if (list_length(clause) != 2) {
          LOG_ERROR("Conditional pair length was %d, not 2.", list_length(clause));
          error = true;
          break;
        }
        obj *predicate = eval(CAR(clause), interpreter);
        if (is_primitive(predicate)) {
          LOG_ERROR("Cannot cast primitive function as bool.");
          error = true;
          break;
        }
        if (!is_nil(predicate)) {
          o = ith(clause, 1);
          tail_call = true;
          break;
        }
      }
      if (tail_call) continue;
      if (!error) result = nil(&interpreter->gc);
      break;
    }

    obj *oper = eval(CAR(o), interpreter);
    if (oper == NULL) break;

    if (is_primitive(oper)) {
      result = PRIMITIVE(oper)(CDR(o), interpreter);
      break;
    }

    if (is_closure(oper)) {
      const obj *args = CDR(o);
      if (NARGS(oper) >= 0) {
        if (!CHECK_NARGS_MAX(args, NARGS(oper))) break;
        if (list_length(args) < NARGS(oper)) {
          result = closure_partial_application(oper, args, interpreter);
          break;
        }
      }
      obj *frame = associate(PARAMETERS(oper), args, interpreter, true);
      if (caller_env == NULL) caller_env = interpreter->env;
      interpreter->env = join_lists(frame, CAPTURED(oper));
      o = PROCEDURE(oper);
      continue;  // tail call
    }

    if (is_macro(oper)) {
      const obj *args = CDR(o);
      if (NARGS(oper) >= 0 && !CHECK_NARGS(args, NARGS(oper))) break;

      obj *frame = associate(PARAMETERS(oper), args, interpreter, false);
      obj *macro_env = join_lists(frame, CAPTURED(oper));

      obj *saved = interpreter->env;
      interpreter->env = macro_env;
      obj *expansion = eval(PROCEDURE(oper), interpreter);
      interpreter->env = saved;

      if (expansion == NULL) break;
      o = expansion;
      continue;  // tail call: eval expansion
    }

    if (is_atom(oper))
      LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(oper));
    else
      LOG_ERROR("Non-procedure cannot be applied");
    break;
  }

  if (caller_env != NULL) interpreter->env = caller_env;
  return result;
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

    obj *old_env = interpreter->env;
    obj *frame = associate(PARAMETERS(oper), args, interpreter, true);
    interpreter->env = join_lists(frame, CAPTURED(oper));
    obj *result = eval(PROCEDURE(oper), interpreter);
    interpreter->env = old_env;
    return result;
  }

  if (is_macro(oper)) {
    if (NARGS(oper) >= 0 && !CHECK_NARGS(args, NARGS(oper))) return NULL;

    obj *frame = associate(PARAMETERS(oper), args, interpreter, false);
    obj *macro_env = join_lists(frame, CAPTURED(oper));

    obj *old_env = interpreter->env;
    interpreter->env = macro_env;
    obj *expansion = eval(PROCEDURE(oper), interpreter);
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
