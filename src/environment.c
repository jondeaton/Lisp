/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include <environment.h>
#include <parser.h>
#include <string.h>
#include <list.h>

#define NUMBUILTINS 8
#define QUOTE_RESV "quote"
#define ATOM_RESV "atom"
#define EQ_RESV "eq"
#define CAR_RESV "car"
#define CDR_RESV "cdr"
#define CONS_RESV "cons"
#define COND_RESV "cond"
#define SET_RESV "set"

static atom_t primitive_names[NUMBUILTINS] = {
  QUOTE_RESV,
  ATOM_RESV,
  EQ_RESV,
  CAR_RESV,
  CDR_RESV,
  CONS_RESV,
  COND_RESV,
  SET_RESV,
};

// Static function declarations
static void insert_primitives(obj *pair_list);
static void replace_primitive_placeholder(obj *pair);
static obj* wrap_primitive(primitive_t primitive);

static obj* makeFuncPair(atom_t a, void* fp);
static ssize_t index_of(char *query, char **strings, size_t num_strings);
static primitive_t lookup_primitive(atom_t atm);

static primitive_t kFuncPts[NUMBUILTINS] = {&quote, &atom, &eq, &car, &cdr, &cons, &cond, &set};
static expression kEnvExp = "((quote x) (atom x) (eq x) (car x) (cdr x) (cons x) (cond x) (set x))";

obj* init_env() {
  size_t unused;
  obj* env = parse_expression(kEnvExp, &unused); // cheeky
  if (env == NULL) return NULL; // ERROR
  insert_primitives(env);
  return env;
}

obj* make_pair(const obj* name, const obj* value) {
  obj* first_item = new_list();
  list_of(first_item)->car = copy(name);

  obj* second_item = new_list();
  list_of(second_item)->car = copy(value);

  list_of(first_item)->cdr = second_item;

  return first_item;
}


obj** lookup_entry(const obj* o, const obj* env) {
  if (o == NULL || env == NULL) return NULL;

  // Error: The environment should be a list
  if (env->objtype != list_obj) return NULL;

  // Error: Can't lookup something that isn't an atom
  if (o->objtype != atom_obj) return NULL;

  // Get the list
  obj* pair = list_of(env)->car;

  if (strcmp(atom_of(o), atom_of(list_of(pair)->car)) == 0)
    return &list_of(list_of(pair)->cdr)->car;

  else return lookup_entry(o, list_of(env)->cdr);
}


obj* lookup(const obj* o, const obj* env) {
  obj** entry = lookup_entry(o, env);
  return entry ? *entry : NULL;
}

/**
 * Function: insert_primitives
 * ---------------------------
 * Replaces the second element of the pairs
 * @param pair_list: The list of pairs to insert primitives into
 */
static void insert_primitives(obj *pair_list) {
  if (pair_list == NULL) return;
  obj* pair = list_of(pair_list)->car;
  replace_primitive_placeholder(pair);
  insert_primitives(list_of(pair_list)->cdr);
}

/**
 * Function: replace_primitive_placeholder
 * ---------------------------------------
 * Replaces the second element of a name-primitive pair with a primitive object
 * corresponding to the name stored in the first element of the pair.
 * @param pair: A pointer to a two element list of an atom with the name of the primitive and a
 * placeholder second item that will be DISPOSED of and replaced with a primitive object
 * corresponding to the name in the first element.
 */
static void replace_primitive_placeholder(obj *pair) {
  if (pair == NULL) return;
  atom_t primitive_name = atom_of(list_of(pair)->car);
  primitive_t primitive = lookup_primitive(primitive_name);

  obj* second = list_of(pair)->cdr;
  dispose(list_of(second)->car);
  list_of(second)->car = wrap_primitive(primitive);
}

/**
 * Function: wrap_primitive
 * ------------------------
 * Wraps the provided primitive in a primitive object in dynamically allocated memory
 * @param primitive : A primitive to wrap in an object
 * @return : A pointer to the object in dynamically allocated memory
 */
static obj* wrap_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  o->objtype = primitive_obj;
  primitive_t* primp = primitive_of(o);
  *primp = primitive;
  return o;
}

/**
 * Function: lookup_primitive
 * --------------------------
 * Get the function pointer to the primitive that the atom refers to by name
 * @return : A primitive function pointer if the atom is found, NULL otherwise
 */
static primitive_t lookup_primitive(atom_t atm) {
  ssize_t i = index_of(atm, primitive_names, NUMBUILTINS);
  if (i == -1) return NULL;
  return kFuncPts[i];
}

/**
 * Function: index_of
 * ------------------
 * Determines which (if any) string in a list of strings match some query string.
 * @param query: Query string
 * @param strings: List of strings to search through
 * @param num_strings: Number of strings in the search list
 * @return: Index of the matching string, or -1 if no match
 */
static ssize_t index_of(char *query, char **strings, size_t num_strings) {
  for(size_t i = 0; i < num_strings; i++) {
    if (strcmp(query, strings[i]) == 0) return i;
  }
  return -1;
}