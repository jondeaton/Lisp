/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include <environment.h>
#include <parser.h>
#include <string.h>
#include <list.h>

#define NUMBUILTINS 7
#define QUOTE_RESV "quote"
#define ATOM_RESV "atom"
#define EQ_RESV "eq"
#define CAR_RESV "car"
#define CDR_RESV "cdr"
#define CONS_RESV "cons"
#define COND_RESV "cond"

static atom_t primitive_names[NUMBUILTINS] = {
  QUOTE_RESV,
  ATOM_RESV,
  EQ_RESV,
  CAR_RESV,
  CDR_RESV,
  CONS_RESV,
  COND_RESV,
};

// Static function declarations
static void insertPrimitives(obj* pairList);
static void replacePrimitivePlaceholder(obj* pair);
static obj* wrapPrimitive(primitive_t primitive);

static obj* makeFuncPair(atom_t a, void* fp);
static ssize_t indexof(char* str, char* strings[], size_t numStrings);
static primitive_t lookupPrimitive(atom_t atm);

static primitive_t kFuncPts[NUMBUILTINS] = {&quote, &atom, &eq, &car, &cdr, &cons, &cond};
static expression kEnvExp = "((quote x) (atom x) (eq x) (car x) (cdr x) (cond x))";

obj* initEnv() {
  size_t unused;
  obj* env = parse_expression(kEnvExp, &unused); // cheeky
  if (env == NULL) return NULL; // ERROR
  insertPrimitives(env);
  return env;
}

/**
 * Function: insertPrimitives
 * --------------------------
 * Replaces the second element of the pairs
 * @param env
 */
static void insertPrimitives(obj* pairList) {
  if (pairList == NULL) return;
  obj* pair = get_list(pairList)->car;
  replacePrimitivePlaceholder(pair);
  insertPrimitives(get_list(pairList)->cdr);
}

/**
 * Function: replacePrimitivePlaceholder
 * -------------------------------------
 * Replaces the second element of a name-primitive pair with a primitive object
 * corresponding to the name stored in the first element of the pair.
 * @param pair : A pointer to a two element list of an atom with the name of the primitive and a
 * placeholder second item that will be DISPOSED of and replaced with a primitive object
 * corresponding to the name in the first element.
 */
static void replacePrimitivePlaceholder(obj* pair) {
  if (pair == NULL) return;
  atom_t primitiveName = get_atom(get_list(pair)->car);
  primitive_t primitive = lookupPrimitive(primitiveName);

  obj* second = get_list(pair)->cdr;
  dispose(get_list(second)->car);
  get_list(second)->car = wrapPrimitive(primitive);
}

/**
 * Function: wrapPrimitive
 * -----------------------
 * Wraps the provided primitive in a primitive object in dynamically allocated memory
 * @param primitive : A primitive to wrap in an object
 * @return : A pointer to the object in dynamically allocated memory
 */
static obj* wrapPrimitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj) + sizeof(primitive_t));
  o->objtype = primitive_obj;
  primitive_t* primp = get_primitive(o);
  *primp = primitive;
  return o;
}

/**
 * Function: lookupPrimitive
 * -------------------------
 * Get the function pointer to the primitive that the atom refers to by name
 * @return : A primitive function pointer if the atom is found, NULL otherwise
 */
static primitive_t lookupPrimitive(atom_t atm) {
  ssize_t i = indexof(atm, primitive_names, NUMBUILTINS);
  if (i == -1) return NULL;
  return kFuncPts[i];
}

/**
 * Function: indexof
 * -----------------
 * Determines which (if any) string in a list of strings match
 * some query string.
 * @param str : Query string
 * @param strings : List of strings to search through
 * @param numStrings : Number of strings in the search list
 * @return : Index of the matching string, or -1 if no match
 */
static ssize_t indexof(char* str, char* strings[], size_t numStrings) {
  for(size_t i = 0; i < numStrings; i++) {
    if (strcmp(str, strings[i]) == 0) return i;
  }
  return -1;
}