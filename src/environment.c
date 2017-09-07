/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include <environment.h>
#include <parser.h>
#include <string.h>

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
static obj* makeFuncPair(atom_t a, void* fp);
static ssize_t indexof(char* str, char* strings[], size_t numStrings);

static void* kFuncPts[NUMBUILTINS] = {quote, atom, eq, car, cdr, cons, cond};
static expression_t kEnvExp = "((quote x) (atom x) (eq x) (car x) (cdr x) (cond x))";
obj* initEnv() {
  size_t unused;
  obj* envp = parseExpression(kEnvExp, &unused); // cheeky
  if (envp == NULL) return NULL; // ERROR

  list_t* l = getList(envp);

  while (l->cdr != NULL) {
    obj* pair = l->car;
    list_t* pairList = getList(envp);
    dispose(pairList->cdr);

    obj* func_obj = calloc(1, sizeof(obj));
    func_obj->objtype = primitive_obj;

    ssize_t i = indexof(getAtom(pairList->car), primitive_names, NUMBUILTINS);

    primitive_t f = getPrimitive(func_obj);

//    *f = kFuncPts[i];
    pairList->cdr = func_obj;

    l = getList(l->cdr);
  }
  return envp;
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