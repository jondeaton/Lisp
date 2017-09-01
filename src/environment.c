/*
 * File: environment.c
 * -------------------
 * Presents the implementation of the default lisp environment
 */

#include <environment.h>

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

static const void* func_ps[NUMBUILTINS] = {quote, atom, eq, car, cdr, cons, cond, lambda};
static const expression_t env_exp = "((quote x) (atom x) (eq x) (car x) (cdr x) (cond x) (lambda x))";


static obj* makeFuncPair(atom_t a, void* fp);
static size_t indexof(char* str, char* strings[]);

obj* initEnv() {
  envp = parse(env_exp, NULL);

  list_t* l = envp->p;

  while (l->cdr != NULL) {
    obj* pair = l->car;
    list_t* pairList = pair->p;
    dispose(pairList->cdr);

    obj* func_obj = calloc(1, sizeof(obj));
    func_obj->objtype = primitive_obj;
    func_obj->p = func_ps[indexof(pairList->car->p, primitive_names)];
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
 *
 * @param strings
 * @param str
 * @return
 */
static size_t indexof(char* str, char* strings[]) {
  for(size_t i = 0; i < numStrings; i++) {
    if (strcmp(str, strings[i])) return i;
  }
}