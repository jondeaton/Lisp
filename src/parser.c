/*
 * File: parser.c
 * --------------
 * Presents the implementation of the lisp parser
 */

#include "list.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>

static obj* parse_atom(const_expression e, size_t *numParsedP);
static obj* parse_list(const_expression e, size_t *numParsedP);
static obj* get_quote_list();
static obj* put_into_list(obj *o);
static bool is_empty_list(const obj *o);
static obj* to_empty_atom(obj *o);
static obj* get_empty_atom();

static expression unparse_list(const obj *o);
static expression unparse_atom(const obj *o);
static expression unparse_primitive(const obj *o);

static size_t atom_size(const_expression e);
static bool is_white_space(char character);
static int distance_to_next_element(const_expression e);

obj* parse_expression(const_expression e, size_t *num_parsed_p) {
  ssize_t start = distance_to_next_element(e);
  if (start == -1) {
    if (num_parsed_p != NULL) *num_parsed_p = strlen(e);
    return NULL;
  }
  expression expr_start = (char*) e + start;

  if (expr_start[0] == ')') {
    if (num_parsed_p != NULL) *num_parsed_p = (size_t) start + 1;
    return NULL;
  } // End of list

  obj* o;
  size_t expr_size;

  if (expr_start[0] == '\'') { // Expression starts with quote charactere
    o = get_quote_list();
    obj* quoted = parse_expression((char *) expr_start + 1, &expr_size);
    expr_size += 1; // for the quote character
    get_list(o)->cdr = put_into_list(quoted);

  } else if (expr_start[0] == '(')  { // Expression starts with opening paren
    o = parse_list((char *) expr_start + 1, &expr_size);
    expr_size += 1; // for the opening parentheses character
    if (o == NULL) o = to_empty_atom(o);

  } else {
    o = parse_atom(expr_start, &expr_size);
  }

  if (num_parsed_p != NULL) *num_parsed_p = start + expr_size;
  return o;
}

expression unparse(const obj* o) {
  if (o == NULL) return NULL;

  if (o->objtype == atom_obj) return unparse_atom(o);

  if (o->objtype == primitive_obj) return unparse_primitive(o);

  if (o->objtype == list_obj) {
    expression listExp = unparse_list(o);
    if (listExp == NULL) return strdup("()");

    expression e = malloc(1 + strlen(listExp) + 2); // open, close, null
    e[0] = '(';
    strcpy((char*) e + 1, listExp);
    strcpy((char*) e + 1 + strlen(listExp), ")");
    return e;
  }
  return NULL;
}

/**
 * Function: unparse_list
 * ----------------------
 * Turn a list into an expression that represents that list. Note: the produced lisp
 * expression will be in dynamically allocated space and will NOT contain opening and closing
 * parentheses.
 * @param o: A lisp object that is a list to be unparsed
 * @return: A lisp expression that represents the passed lisp object
 */
static expression unparse_list(const obj *o) {
  if (o == NULL) return NULL;

  expression e;

  expression carExp = unparse(get_list(o)->car);
  if (carExp == NULL) return NULL;
  expression cdrExp = unparse_list(get_list(o)->cdr);

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
 * Function: unparse_atom
 * ----------------------
 * Unparses an atom into dynamically allocated
 * @param o: Pointer to an atom object
 * @return: Pointer to dynamically allocated memory with the an expression representing the atom
 */
static expression unparse_atom(const obj *o) {
  if (o == NULL) return NULL;
  atom_t atm = get_atom(o);
  expression e = malloc(strlen(atm) + 1);
  return strcpy(e, atm);
}

/**
 * Function: unparse_primitive
 * ---------------------------
 * Turns a primitive atom into a string in dynamically allocated memory
 * @param o: A pointer to a lisp object of primitive type
 * @return: An expression in dynamically allocated memory that
 */
static expression unparse_primitive(const obj *o) {
  if (o == NULL) return NULL;
  expression e = malloc(2 + sizeof(void*) * 16 + 1);
  sprintf(e, "%p", *get_primitive(o)); // just print the raw pointer
  return e;
}

/**
 * Function: is_balanced
 * ---------------------
 * Determines if an expression has balanced parenthesis
 * @param e: The lisp expression to check
 * @return: True if each opening parentheses in the expression is balanced by a closing parentheses
 * and there are no extra closing parentheses, false otherwise.
 */
bool is_balanced(const_expression e) {
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
bool is_valid(const_expression e) {
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
static obj* parse_atom(const_expression e, size_t *numParsedP) {
  size_t size = atom_size(e);
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
static obj* parse_list(const_expression e, size_t *numParsedP) {
  int start = distance_to_next_element(e);
  expression exprStart = (char*) e + start;

  if (exprStart[0] == ')') {
    *numParsedP = (size_t) start + 1;
    return NULL;
  } // Empty list or the end of a list

  size_t exprSize;
  obj* nextElement = parse_expression(exprStart, &exprSize); // will find closing paren
  obj* o = put_into_list(nextElement);

  size_t restSize;
  expression restOfList = (char*) exprStart + exprSize;
  get_list(o)->cdr = parse_list(restOfList, &restSize);

  *numParsedP = start + exprSize + restSize;
  return o;
}

/**
 * Function: getQuoteList
 * ----------------------
 * Creates a list where car points to a "quote" atom and cdr points to nothing
 * @return : Pointer to the list object
 */
static obj* get_quote_list() {
  size_t i;
  obj* quote_atom = parse_atom("quote", &i);
  if (quote_atom == NULL) return NULL;
  return put_into_list(quote_atom);
}

/**
 * Function: putIntoList
 * ---------------------
 * Makes a list object with car pointing to the object passed
 * @param o : The object that the list's car should point to
 * @return : A pointer to the list object containing only the argument object
 */
static obj* put_into_list(obj *o) {
  obj* listObj = calloc(1, sizeof(obj) + sizeof(list_t));
  if (listObj == NULL) return NULL;
  listObj->objtype = list_obj;
  get_list(listObj)->car = o;
  return listObj;
}

/**
 * Function: is_empty_list
 * -----------------------
 * Determines if a list object is an empty list. Note: this is for checking
 * if the object is a list object that is empty, which is NOT the same thing as
 * checking if the list object is the empty-list atom.
 * @param o: A list object to check
 * @return: True if the object is a list object that is empty, false otherwise
 */
static bool is_empty_list(const obj *o) {
  if (o->objtype != list_obj) return false;

  list_t* l = get_list(o);
  return l->car == NULL && l->cdr == NULL;
}

/**
 * Function: to_empty_atom
 * -----------------------
 * Frees the passed object and returns the empty list atom
 * @param o: Object to be discarded
 * @return: Empty list atom object
 */
static obj* to_empty_atom(obj *o) {
  free(o);
  return get_empty_atom();
}

/**
 * Function: getEmptyAtom
 * ----------------------
 * Makes a new empty list atom
 * @return : Pointer to a new empty list atom in dynamically allocated memory
 */
static obj* get_empty_atom() {
  obj* atomObj = calloc(1, sizeof(obj) + 3);
  atomObj->objtype = atom_obj;
  strcpy(get_atom(atomObj), "()");
  return atomObj;
}

/**
 * Function: distance_to_next_element
 * ----------------------------------
 * Counts the number of characters of whitespace until a non-whitespace character is found
 * @param e: A lisp expression
 * @return: The number of characters of whitespace in the beginning
 */
static int distance_to_next_element(const_expression e) {
  size_t i;
  for (i = 0; i < strlen(e); i++)
    if (!is_white_space(e[i])) break;
  if (i == strlen(e)) return -1;
  return i;
}

/**
 * Function: atom_size
 * -------------------
 * Finds the size of the atom pointed to in the expression
 * @param e: An expression that represents an atom
 * @return: The number of characters in that atom
 */
static size_t atom_size(const_expression e) {
  size_t i;
  for(i = 0; i < strlen(e); i++) {
    if (is_white_space(e[i]) || e[i] == '(' || e[i] == ')') return i;
  }
  return i;
}

/**
 * Function: is_white_space
 * ------------------------
 * Checks if a single character is a whitespace character
 * @param character : The character to check
 * @return : True if that character is whitespace, false otherwise
 */
static const char* kWhitespace = " \t\n";
static bool is_white_space(char character) {
  return strchr(kWhitespace, character) != NULL;
}
