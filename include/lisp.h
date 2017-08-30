/**
 * File: lisp.h
 * ------------
 * Presents the lisp primitives interface
 */

#ifndef _lisp_
#define _lisp_

#include "stdbool.h"
#include "stdlib.h"

typedef struct {
	bool isatom;
	void* p;
} expression;

typedef char* atom;

atom t = "t";
atom empty = NULL;

typedef struct {
	expression* car;
	expression* cdr;
} list;

/**
 * Primitive: quote
 * ----------------
 *
 * @param atom
 * @return
 */
atom quote(expression* atom);

/**
 * Primitive: atom
 * ---------------
 *
 * @param e
 * @return
 */
atom atom(expression* e);

/**
 * Primitive: eq
 * -------------
 *
 * @param x
 * @param y
 * @return
 */
atom eq(expression* x, expression* y);


list* car(list* l);

list* cdr(list* l);

list* cons(expression* x, list* y);

/**
 * Primitive: cons
 * ---------------
 * (cond (p1 e1) ... (pn en))
 * The p expressions are evaluated in order until one returns t
 * When one is found  the value of the corresp onding e expression
 * is returned as the expression
 * @param exps
 * @return
 */
expression* cond(list* exps);

#endif