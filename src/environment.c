/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include <environment.h>
#include <parser.h>
#include <string.h>

#define NUMBUILTINS 8
#define QUOTE_RESV "quote"
#define ATOM_RESV "atom"
#define EQ_RESV "eq"
#define CAR_RESV "car"
#define CDR_RESV "cdr"
#define CONS_RESV "cons"
#define COND_RESV "cond"
#define LAMBDA_RESV "lambda"

obj* envp;

static const atom_t primitive_names[NUMBUILTINS] = {
  QUOTE_RESV,
  ATOM_RESV,
  EQ_RESV,
  CAR_RESV,
  CDR_RESV,
  CONS_RESV,
  COND_RESV,
  LAMBDA_RESV
};

// Static function declarations
static obj* makeFuncPair(atom_t a, void* fp);
static ssize_t indexof(char* str, char* strings[], size_t numStrings);

static const void* kFuncPts[NUMBUILTINS] = {quote, atom, eq, car, cdr, cons, cond, lambda};
static const expression_t kEnvExp = "((quote x) (atom x) (eq x) (car x) (cdr x) (cond x) (lambda x))";
obj* initEnv() {
  envp = parse(kEnvExp, NULL); // cheeky
  if (envp == NULL) return NULL; // ERROR

  list_t* l = envp->p;

  while (l->cdr != NULL) {
    obj* pair = l->car;
    list_t* pairList = pair->p;
    dispose(pairList->cdr);

    obj* func_obj = calloc(1, sizeof(obj));
    func_obj->objtype = primitive_obj;
    func_obj->p = kFuncPts[indexof(pairList->car->p, primitive_names, NUMBUILTINS)];
    pairList->cdr = func_obj;

    l = l->cdr->p;
  }
  return envp;
}

void disposeEnv() {
  dispose(envp);
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
}