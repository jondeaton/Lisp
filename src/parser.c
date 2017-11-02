/*
 * File: parser.c
 * --------------
 * Presents the implementation of the lisp parser
 */

#include <parser.h>
#include <string.h>
#include <stdio.h>

static obj* parseAtom(expression_t e, size_t* numParsedP);
static obj* parseList(expression_t e, size_t* numParsedP);
static obj* getQuoteList();
static obj* putIntoList(obj* o);
static bool isEmptyList(obj *o);
static obj* toEmptyAtom(obj* o);
static obj* getEmptyAtom();

static expression_t unparseList(obj* o);
static expression_t unparseAtom(obj* o);
static expression_t unparsePrimitive(obj* o);

  static size_t atomSize(expression_t e);
static bool isWhiteSpace(char character);
static int findNext(expression_t e);

obj* parseExpression(expression_t e, size_t* numParsedP) {
  ssize_t start = findNext(e);
  if (start == -1) {
    *numParsedP = strlen(e);
    return NULL;
  }
  expression_t exprStart = (char*) e + start;

  if (exprStart[0] == ')') {
    *numParsedP = (size_t) start + 1;
    return NULL;
  } // End of list

  obj* o;
  size_t exprSize;

  if (exprStart[0] == '\'') {
    o = getQuoteList();
    obj* quoted = parseExpression((char*) exprStart + 1, &exprSize);
    getList(o)->cdr = putIntoList(quoted);

  } else if (exprStart[0] == '(')  {
    o = parseList((char*) exprStart + 1, &exprSize);
    exprSize += 1;
    if (o == NULL) o = toEmptyAtom(o);
  } else  {
    o = parseAtom(exprStart, &exprSize);
  }

  *numParsedP = start + exprSize;
  return o;
}

expression_t unparse(obj* o) {
  if (o == NULL) return NULL;

  if (o->objtype == atom_obj) return unparseAtom(o);

  if (o->objtype == primitive_obj) return unparsePrimitive(o);

  if (o->objtype == list_obj) {
    expression_t listExp = unparseList(o);
    if (listExp == NULL) return strdup("()");

    expression_t e = malloc(1 + strlen(listExp) + 2); // open, close, null
    e[0] = '(';
    strcpy((char*) e + 1, listExp);
    strcpy((char*) e + 1 + strlen(listExp), ")");
    return e;
  }
  return NULL;
}

/**
 * Function: unparseList
 * ---------------------
 * Turn a list into an expression that represents that list. Note: the produced lisp
 * expression will be in dynamically allocated space and will NOT contain opening and closing
 * parentheses.
 * @param o: A lisp object that is a list to be unparsed
 * @return: A lisp expression that represents the passed lisp object
 */
static expression_t unparseList(obj* o) {
  if (o == NULL) return NULL;

  expression_t e;

  expression_t carExp = unparse(getList(o)->car);
  if (carExp == NULL) return NULL;
  expression_t cdrExp = unparseList(getList(o)->cdr);

  size_t carExpSize = strlen(carExp);
  if (cdrExp == NULL) {
    e = calloc(1, 2 + carExpSize);
    if (e == NULL) return NULL;
    strcpy(e, carExp);

  } else {
    size_t cdrExpSize = strlen(cdrExp);
    e = calloc(1, carExpSize + 1 + cdrExpSize + 1);
    if (e == NULL) return NULL;

    strcpy(e, carExp);
    strcpy((char*) e + carExpSize, " ");
    strcpy((char*) e + carExpSize + 1, cdrExp);
    free(cdrExp);
  }
  free(carExp);
  return e;
}

/**
 * Function: unparseAtom
 * ---------------------
 * Unparses an atom into dynamically allocated
 * @param o : Pointer to an atom object
 * @return : Pointer to dynamically allocated memory with the an expression representing the atom
 */
static expression_t unparseAtom(obj* o) {
  if (o == NULL) return NULL;
  atom_t atm = getAtom(o);
  expression_t e = malloc(strlen(atm) + 1);
  return strcpy(e, atm);
}

/**
 * Function: unparsePrimitive
 * --------------------------
 * Turns a primitive atom into a string in dynamically allocated memory
 * @param o : A pointer to a lisp object of primitive type
 * @return : An expression in dynamically allocated memory that
 */
static expression_t unparsePrimitive(obj* o) {
  if (o == NULL) return NULL;
  expression_t e = malloc(2 + sizeof(void*) * 16 + 1);
  sprintf(e, "%p", *getPrimitive(o));
  return e;
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
 * @param e : A pointer to an atom expression
 * @param numParsedP : Pointer to a location to be populated with the number of characters parsed
 * @return : A lisp object representing the parsed atom in dynamically allocated memory
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
  int start = findNext(e);
  expression_t exprStart = (char*) e + start;

  if (exprStart[0] == ')') {
    *numParsedP = (size_t) start + 1;
    return NULL;
  } // Empty list or the end of a list

  size_t exprSize;
  obj* nextElement = parseExpression(exprStart, &exprSize); // will find closing paren
  obj* o = putIntoList(nextElement);

  size_t restSize;
  expression_t restOfList = (char*) exprStart + exprSize;
  getList(o)->cdr = parseList(restOfList, &restSize);

  *numParsedP = start + exprSize + restSize;
  return o;
}

/**
 * Function: getQuoteList
 * ----------------------
 * Creates a list where car points to a "quote" atom and cdr points to nothing
 * @return : Pointer to the list object
 */
static obj* getQuoteList() {
  size_t i;
  obj* quote_atom = parseAtom("quote", &i);
  if (quote_atom == NULL) return NULL;
  return putIntoList(quote_atom);
}

/**
 * Function: putIntoList
 * ---------------------
 * Makes a list object with car pointing to the object passed
 * @param o : The object that the list's car should point to
 * @return : A pointer to the list object containing only the argument object
 */
static obj* putIntoList(obj* o) {
  obj* listObj = calloc(1, sizeof(obj) + sizeof(list_t));
  if (listObj == NULL) return NULL;
  listObj->objtype = list_obj;
  getList(listObj)->car = o;
  return listObj;
}

/**
 * Function: isEmptyList
 * ---------------------
 * Determines if a list object is an empty list. Note: this is for checking
 * if the object is a list object that is empty, which is NOT the same thing as
 * checking if the list object is the empty-list atom.
 * @param o: A list object to check
 * @return: True if the object is a list object that is empty, false otherwise
 */
static bool isEmptyList(obj *o) {

  //
  if (o->objtype != list_obj) return false;

  list_t* l = getList(o);

  return l->car == NULL && l->cdr == NULL;
}

/**
 * Function: toEmptyAtom
 * ---------------------
 * Frees the passed object and returns the empty list atom
 * @param o : Object to be discarded
 * @return : Empty list atom object
 */
static obj* toEmptyAtom(obj* o) {
  free(o);
  return getEmptyAtom();
}

/**
 * Function: getEmptyAtom
 * ----------------------
 * Makes a new empty list atom
 * @return : Pointer to a new empty list atom in dynamically allocated memory
 */
static obj* getEmptyAtom() {
  obj* atomObj = calloc(1, sizeof(obj) + 3);
  atomObj->objtype = atom_obj;
  strcpy(getAtom(atomObj), "()");
  return atomObj;
}

/**
 * Function: findNext
 * -------------------
 * Counts the number of characters of whitespace until a non-whitespace character is found
 * @param e : A lisp expression
 * @return : The number of characters of whitespace in the beginning
 */
static int findNext(expression_t e) {
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
