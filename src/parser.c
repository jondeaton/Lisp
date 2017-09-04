/*
 * File: parser.c
 * --------------
 * Presents the implementation of the lisp parser
 */

#include <parser.h>
#include <string.h>
#include <list.h>

#define UNPARSE_BUFF 16

static obj* parseAtom(expression_t e, size_t* numParsedP);
static obj* parseList(expression_t e, size_t* numParsedP);

static size_t atomSize(expression_t e);
static bool isWhiteSpace(char character);
static ssize_t findStart(expression_t e);

/**
 * Function: parseExpression
 * -------------------------
 * Parses a lisp expression that represents either a lisp atom or list
 * @param e : A balanced, valid lisp expression
 * @param numParsedP : A pointer to a place where the number of parsed characters may be written. Must be valid
 * @return : Pointer to a lisp data structure object representing that the lisp expression represents
 */
obj* parseExpression(expression_t e, size_t* numParsedP) {

  ssize_t i = findStart(e);
  if (i == -1) {
    if (numParsedP != NULL) *numParsedP = strlen(e);
    return NULL;
  }
  expression_t exprStart = (char*) e + i;

  if (e[0] == ')') {
    if (numParsedP != NULL) *numParsedP = 1;
    return NULL;
  } // End of list

  if (exprStart[0] == '(') return parseList((char*) exprStart + 1, numParsedP);
  else return parseAtom(exprStart, numParsedP);
}

expression_t unparse(obj* o) {
  if (o == NULL) return NULL;

  if (o->objtype == atom_obj) {
    atom_t atm = getAtom(o);
    expression_t e = malloc(strlen(atm) + 1);
    return strcpy(e, atm);
  }

  if (o->objtype == list_obj) {
    expression_t e = malloc(UNPARSE_BUFF);
    if (e == NULL) return NULL;
    e[0] = '(';

    list_t* l = getList(o);
    expression_t carExp = unparse(l->car);
    if (carExp == NULL) {
      e[1] = ')';
      return e;
    }

    size_t carExpSize = strlen(carExp);

    // 4: open paren, space, close paren, null terminator
    if (1 + carExpSize + 3 > UNPARSE_BUFF) {
      e = realloc(e, 4 + carExpSize);
      if (e == NULL) return NULL;
    }
    strcpy(e + 1, carExp);
    free(carExp);
    e[1 + carExpSize] = ' ';

    expression_t cdrExp = unparse(l->cdr);
    if (cdrExp == NULL) {
      e[1 + carExpSize] = ')';
      return e;
    }
    size_t cdrExpSize = strlen(cdrExp);

    if (1 + carExpSize + 1 + cdrExpSize + 2 > 4 + carExpSize) {
      e = realloc(e, 4 + carExpSize + cdrExpSize);
      if (e == NULL) return NULL;
    }
    strcpy(e + 2 + carExpSize, cdrExp);
    free(cdrExp);

    strcpy(e + 1 + carExpSize + 1 + cdrExpSize, ")");
    return e;
  }
  return NULL;
}

/**
 * Function: isBalanced
 * --------------------
 * Determines if an expression has balanced parenthesis
 * @param e : The lisp expression to check
 * @return : True if each opening parentheses in the expression is balanced by a closing parentheses
 * and there are no extra closing parentheses, false otherwise.
 */
bool isBalanced(expression_t e) {
  int net = 0;
  for (size_t i = 0; i < strlen(e); i++) {
    if (e[i] == '(') net++;
    if (e[i] == ')') net--;
  }
  return net == 0;
}

/**
 * Function: isValid
 * -----------------
 * Determines if an expression has extra closing parentheses
 * @param e : A lisp expression
 * @return : True is there are no extra closing parentheses, false otherwise
 */
bool isValid(expression_t e) {
  int net = 0;
  for (size_t i = 0; i < strlen(e); i++) {
    if (e[i] == '(') net++;
    if (e[i] == ')') net--;
    if (net < 0) return false;
  }
  return net >= 0;
}

/**
 * Function: parseAtom
 * -------------------
 * Parses an expression that represents an atom
 * @param e :
 * @param numParsedP
 * @return
 */
static obj* parseAtom(expression_t e, size_t* numParsedP) {
  size_t size = atomSize(e);
  obj* o = calloc(1, sizeof(obj) + size + 1);
  if (o == NULL) return NULL; // fuck me right?

  atom_t atm = (char*) o + sizeof(obj);
  strncpy(atm, e, size);
  *numParsedP = size;
  return o;
}

/**
 * Function: parseList
 * -------------------
 * Parses an expression that represents a list. This expression should not start
 * with an opening parentheses. This function will parse until there is a closing parentheses
 * that closes the implicit opening parentheses. Note: this is NOT necessarily the first closing
 * parentheses as there may be lists nested inside of this list.
 * @param e : An expression representing a list
 * @param numParsedP : A pointer to a place where the number of parsed characters may be written. Must be valid
 * @return : Pointer to a lisp data structure object representing the lisp expression
 */
static obj* parseList(expression_t e, size_t* numParsedP) {
  obj* o = calloc(1, sizeof(obj) + sizeof(list_t));
  if (o == NULL) return NULL; // fuck me right?

  o->objtype = list_obj;
  list_t* l = getList(o);

  size_t carExprSize;
  l->car = parseExpression(e, &carExprSize);

  if (l->car == NULL) { // THE LIST ENDED
    *numParsedP = carExprSize;
    return o;
  }

  size_t cdrExprSize;
  l->cdr = parseList((char*)e + carExprSize, &cdrExprSize);
  return o;
}

/**
 * Function: findStart
 * -------------------
 * Counts the number of characters of whitespace until a non-whitespace character is found
 * @param e : A lisp expression
 * @return : The number of characters of whitespace in the beginning
 */
static ssize_t findStart(expression_t e) {
  size_t i;
  for (i = 0; i < strlen(e); i++)
    if (!isWhiteSpace(e[i])) break;
  if (i == strlen(e)) return -1;
  return i;
}

/**
 * Function: atomSize
 * ------------------------
 * Finds the size of the atom pointed to in the expression
 * @param e : An expression that represents an atom
 * @return : The number of characters in that atom
 */
static size_t atomSize(expression_t e) {
  size_t i;
  for(i = 0; i < strlen(e); i++) {
    if (isWhiteSpace(e[i]) || e[i] == '(' || e[i] == ')') return i;
  }
  return i;
}

/**
 * Function: isWhiteSpace
 * ----------------------
 * Checks if a single character is a whitespace character
 * @param character : The character to check
 * @return : True if that character is whitespace, false otherwise
 */
static const char* kWhitespace = " \t\n";
static bool isWhiteSpace(char character) {
  return strchr(kWhitespace, character) != NULL;
}