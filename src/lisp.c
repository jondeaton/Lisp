#include "lisp.h"
#include "string.h"

atom quote(expression* atom) { return atom; }


atom atom(expression* e) {
	if (e->isatom) return 't';
	else return empty;
}

atom eq(expression* x, expression* y) {

  if (x->isatom && y->isatom)
    return strcmp(x, y) ? t : empty;

	if (eval(x) == eval(y)) return t;
	else return empty;
}

list* car(list* l) { return l->car; }

list* cdr(list* l) { return l->cdr; }

list* cons(expression* x, list* y) {
	list* v = (list*) malloc(sizeof(list));
	v->car = x;

	v->cdr = (expression*) malloc(sizeof(expression));;
  v->cdr->isatom = false;
  v->cdr->p = y;

  return v;
}


expression* cond(list* exps) {
	// todo
}
