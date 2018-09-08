/*
 * File: closures.c
 * ----------------
 * Implements closure related functionality
 */

#include <closure.h>
#include <lisp-objects.h>
#include <memory-manager.h>
#include <list.h>
#include <environment.h>
#include <evaluator.h>
#include <stack-trace.h>

#include <stdlib.h>
#include <interpreter.h>

// Static function declarations

obj *closure_partial_application(const obj *closure, const obj *args, LispInterpreter *interpreter) {
  if (closure == NULL) return NULL;

  int nargs = list_length(args);

  obj* params = copy_recursive(sublist(PARAMETERS(closure), nargs));
  obj* procedure = copy_recursive(PROCEDURE(closure));

  obj* new_bindings = associate(PARAMETERS(closure), args, interpreter);
  obj* captured = join_lists(new_bindings, copy_recursive(CAPTURED(closure)));

  obj* new_closure = new_closure_set(params, procedure, captured);
  mm_add_recursive(&interpreter->mm, new_closure);
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

obj* copy_closure_recursive(const obj* closure) {
  obj *params = copy_recursive(PARAMETERS(closure));
  obj *proc = copy_recursive(PROCEDURE(closure));
  obj *capt = copy_recursive(CAPTURED(closure));
  return new_closure_set(params, proc, capt);
}

obj *associate(obj *names, const obj *args, LispInterpreter *interpreter) {
  if (!is_list(names) || !is_list(args)) return NULL;

  obj* value = eval(CAR(args), interpreter);
  obj* pair = make_pair(CAR(names), value, true);
  obj* cdr = associate(CDR(names), CDR(args), interpreter);
  return new_list_set(pair, cdr);
}
