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
#include <memory-manager.h>
#include <interpreter.h>

// Static function declarations
static obj *bind(obj *params, const obj *args, LispInterpreter *interpreter);

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

  // Numbers, primitives and closures evaluate to themselves
  if (is_number(o) || is_primitive(o) || is_closure(o)) return (obj*) o;

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

    obj* tmp_env = bind(PARAMETERS(oper), args, interpreter); // Bind the parameters to the arguments
    obj* capture_copy = copy_recursive(CAPTURED(oper));
    obj* new_env = join_lists(capture_copy, tmp_env); // Prepend the captured list to the environment

    obj* old_env = interpreter->env; // gotta keep one around in case points is modified in eval
    interpreter->env = new_env;
    obj* result = eval(PROCEDURE(oper), interpreter); // Evaluate body in prepended environment
    interpreter->env = old_env;

    bool split = split_lists(new_env, old_env);
    if (split) mm_add_recursive(&interpreter->mm, new_env); // Mark the bound elements for cleanup

    return result;
  }

  if (is_atom(oper)) {
    LOG_ERROR("Cannot apply atom: \"%s\" as function", ATOM(oper));
    return NULL;
  }
  LOG_ERROR("Non-procedure cannot be applied");
  return NULL;
}

/**
 * Function: bind
 * --------------
 * binds a list of arguments to parameters and prepends them on to an environment
 * @param params: List of parameters
 * @param args: List of arguments to bind to the parameters
 * @param envp: Environment to prepend the bound arguments to
 * @return: Environment now with bound arguments appended
 */
static obj *bind(obj *params, const obj *args, LispInterpreter *interpreter) {
  obj* frame = associate(params, args, interpreter);
  return join_lists(frame, interpreter->env);
}
