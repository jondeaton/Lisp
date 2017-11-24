/*
 * File: closures.c
 * ----------------
 * Presents the implementation of closure related functionality
 */

#include <lisp-objects.h>
#include <closure.h>
#include <garbage-collector.h>
#include <list.h>
#include <environment.h>
#include <stdlib.h>

// Static function declarations
static void get_captured_vars(obj **capturedp, const obj *params, const obj *procedure, const obj *env);
static obj *new_closure_set(obj *params, obj *procedure, obj *captured);

obj* make_closure(const obj *lambda, obj *env) {

  obj* params = copy_recursive(get_lambda_parameters(lambda));
  obj* procedure = copy_recursive(get_lambda_body(lambda));

  obj* captured;
  get_captured_vars(&captured, procedure, params, env);

  obj* closure = new_closure_set(params, procedure, captured);
  add_allocated_recursive(closure);
  return closure;
}

obj* get_lambda_parameters(const obj *lambda) {
  return ith(lambda, 0);
}

obj* get_lambda_body(const obj *lambda) {
  return ith(lambda, 1);
}

/**
 * Function: get_captured_vars
 * ---------------------------
 * Creates a captured variable list by searching for variable names that exist in both the procedure
 * and the environment. Creates a list of key-value pairs extracted (copied) from the environment.
 * Variable names in the parameter list will not be captured.
 * @param capturedp: Pointer to where the captured list reference should be stored
 * @param params: Parameters to the lambda function (these will not be captured
 * @param procedure: Procedure body of the lambda function to search for variables to bind in
 * @param env: Environment to search for values to capture
 */
static void get_captured_vars(obj **capturedp, const obj *params, const obj *procedure, const obj *env) {

  if (is_list(procedure)) { // good ol' depth-first search
    get_captured_vars(capturedp, params, list_of(procedure)->car, env);
    get_captured_vars(capturedp, params, list_of(procedure)->cdr, env);

  } else if (is_atom(procedure)) {
    if (lookup_pair(procedure, *capturedp)) return; // Already captured
    if (list_contains(params, procedure)) return; // Don't capture parameters (those get bound at apply-time)

    obj* matching_pair = lookup_pair(env, procedure);
    if (!matching_pair) return; // No value to be captured
    *capturedp = new_list_set(copy_recursive(matching_pair), *capturedp); // Prepend to capture list
  }

}

/**
 * Function: new_closure_set
 * -------------------------
 * Create a new closure with initialization of fields. The passed objects will not be copied.
 * @param params: Parameters to the closure
 * @param procedure: Procedure of the closure
 * @param captured: Captured argument list of the closure
 * @return: A new closure object with the specified parameters, procedure, and captured vars list.
 */
static obj *new_closure_set(obj *params, obj *procedure, obj *captured) {
  obj* o = new_closure();
  closure_of(o)->parameters = params;
  closure_of(o)->procedure = procedure;
  closure_of(o)->captured = captured;
  return o;
}
