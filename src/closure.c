/*
 * File: closure.c
 * ---------------
 * Implements closure related functionality
 */

#include <closure.h>
#include <lisp-objects.h>
#include <garbage-collector.h>
#include <list.h>
#include <environment.h>
#include <evaluator.h>
#include <stack-trace.h>

#include <stdlib.h>
#include <interpreter.h>

obj *closure_partial_application(const obj *closure, const obj *args, LispInterpreter *interpreter) {
  if (closure == NULL) return NULL;

  int nargs = list_length(args);

  obj *params = sublist(PARAMETERS(closure), nargs);
  obj *procedure = PROCEDURE(closure);

  obj *new_bindings = associate(PARAMETERS(closure), args, interpreter, true);
  obj *captured = join_lists(new_bindings, CAPTURED(closure));

  obj *new_closure = new_closure_set(params, procedure, captured);
  gc_add(&interpreter->gc, new_closure);
  return new_closure;
}

obj *new_closure_set(obj *params, obj *procedure, obj *captured) {
  obj* o = new_closure();
  PARAMETERS(o) = params;
  PROCEDURE(o)  = procedure;
  CAPTURED(o)   = captured;
  if (is_atom(params))
    NARGS(o) = -1;  // variadic
  else
    NARGS(o) = is_nil(params) ? 0 : list_length(params);
  return o;
}

static obj *eval_args_to_list(const obj *args, LispInterpreter *interpreter) {
  if (args == NULL || !is_list(args) || is_nil(args)) return NULL;
  obj *value = eval(CAR(args), interpreter);
  if (value == NULL) return NULL;
  obj *rest = eval_args_to_list(CDR(args), interpreter);
  if (CDR(args) != NULL && rest == NULL) return NULL;
  obj *node = new_list_set(value, rest);
  gc_add(&interpreter->gc, node);
  return node;
}

obj *associate(obj *names, const obj *args, LispInterpreter *interpreter, bool eval_args) {
  if (names == NULL) return NULL;

  // Variadic: atom param captures all remaining args as a list
  if (is_atom(names)) {
    obj *values;
    if (args == NULL || !is_list(args) || is_nil(args))
      values = interpreter->gc.nil_cached;
    else if (eval_args) {
      values = eval_args_to_list(args, interpreter);
      if (values == NULL) return NULL;
    } else
      values = (obj*)args;

    obj *pair = make_pair(names, values);
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, CDR(pair));
    obj *frame = new_list_set(pair, NULL);
    gc_add(&interpreter->gc, frame);
    return frame;
  }

  if (!is_list(names) || !is_list(args)) return NULL;

  obj *value = eval_args ? eval(CAR(args), interpreter) : CAR(args);
  obj *pair = make_pair(CAR(names), value);
  gc_add(&interpreter->gc, pair);
  gc_add(&interpreter->gc, CDR(pair));

  obj* cdr = associate(CDR(names), CDR(args), interpreter, eval_args);
  obj *nested_pair = new_list_set(pair, cdr);
  gc_add(&interpreter->gc, nested_pair);
  return nested_pair;
}
