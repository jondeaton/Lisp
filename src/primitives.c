/*
 * File: lisp.c
 * ------------
 * Presents the implementation of the lisp primitives
 */

#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <garbage-collector.h>
#include <stack-trace.h>
#include <string.h>

static const char* t_contents = "t";
static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "env", "defmacro", NULL };

static const primitive_t primitive_functions[] = { &quote,  &atom,  &eq,  &car,  &cdr,  &cons,
                                                   &cond,  &set, &env_prim, &defmacro,  NULL };

// Static function declarations
static bool is_t(const obj* o);
static obj* ith_arg_value(const obj *args, obj** envp, int i);

obj* get_primitive_library() {
  return make_environment(primitive_reserved_names, primitive_functions);
}

// Allocate new truth atom
obj* t() {
  obj* t = new_atom(t_contents);
  add_allocated(t);
  return t;
}

// Allocate new empty list
obj* empty() {
  obj* list = new_list_set(NULL, NULL);
  add_allocated(list);
  return list;
}

obj* quote(const obj* args, obj** envp) {
  (void) envp;
  if (!CHECK_NARGS(args, 1)) return NULL;
  return list_of(args)->car;
}

obj* atom(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* result = eval(list_of(args)->car, envp);
  if (is_list(result)) return is_empty(result) ? t() : empty();
  return is_atom(result) || is_number(result) ? t() : empty();
}

obj* eq(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = ith_arg_value(args, envp, 0);
  obj* second = ith_arg_value(args, envp, 1);

  bool same = compare(first, second);
  return same ? t() : empty();
}

obj* car(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  if (!check_nargs(__func__, args, 1)) return NULL;
  obj* result = eval(list_of(args)->car, envp);
  return list_of(result)->car;
}

obj* cdr(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 1)) return NULL;

  obj* result = eval(list_of(args)->car, envp);
  return list_of(result)->cdr;
}

obj* cons(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* x = list_of(args)->car;
  obj* y = list_of(list_of(args)->cdr)->car;

  obj* new_obj = new_list();
  add_allocated(new_obj); // Record allocation
  list_of(new_obj)->car = eval(x, envp);
  list_of(new_obj)->cdr = eval(y, envp);

  return new_obj;
}

obj* cond(const obj* o, obj** envp) {
  if (o == NULL) return NULL;

  obj* pair = list_of(o)->car;
  list_t* pl = list_of(pair);

  obj* predicate = eval(pl->car, envp);
  if (is_t(predicate)) {
    obj* e = list_of(pl->cdr)->car;
    return eval(e, envp);
  } else {
    if (pl->cdr == NULL) return empty();
    return cond(list_of(o)->cdr, envp);
  }
}

obj* set(const obj* args, obj** envp) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* var_name = ith_arg_value(args, envp, 0);
  obj* value = ith_arg_value(args, envp, 1);

  obj** prev_value_p = lookup_entry(var_name, *envp);
  if (prev_value_p) { // Dynamic Scoping
    dispose_recursive(*prev_value_p);
    *prev_value_p = copy_recursive(value);

  } else {
    obj* pair_second = new_list_set(copy_recursive(value), NULL);
    obj* pair_first = new_list_set(copy_recursive(var_name), pair_second);
    obj* new_link = new_list_set(pair_first, *envp);
    *envp = new_link;
  }
  return value;
}

obj* env_prim(const obj* args, obj** envp) {
  if (!check_nargs(__func__, args, 0)) return NULL;
  return *envp;
}

obj* defmacro(const obj* args, obj** envp) {
  (void) args;
  (void) envp;
  return LOG_ERROR("Macro definition not yet supported");
}

/**
 * Function: is_t
 * --------------
 * Determines if a lisp object is the truth atom
 * @param o: A lisp object to determine if it is the t atom
 * @return: True if it is the truth atom, false otherwise
 */
static bool is_t(const obj* o) {
  if (o == NULL) return false;
  if (!is_atom(o)) return false;
  return strcmp(atom_of(o), t_contents) == 0;
}

/**
 * Function: ith_arg_value
 * -----------------------
 * Gets the evaluation of the argument at the nth index of the argument list
 * @param args: The list to get the i'th evaluation of
 * @param envp: The environment to do the evaluation in
 * @param i: The index (starting at 0) of the element to evaluate in o
 * @return: The evaluation of the i'th element of o in the given environment
 */
static obj* ith_arg_value(const obj *args, obj** envp, int i) {
  return eval(ith(args, i), envp);
}
