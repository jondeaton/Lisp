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
  NARGS(o)      = is_nil(params) ? 0 : list_length(params);
  return o;
}

obj *associate(obj *names, const obj *args, LispInterpreter *interpreter, bool eval_args) {
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
