/*
 * File: parser.c
 * --------------
 * Presents the implementation of the lisp parser
 */

#include <parser.h>
#include <stack-trace.h>
#include <string.h>
#include <stdio.h>
#include <lisp-objects.h>


#define KMAG  "\x1B[35m"
#define RESET "\033[0m"
#define BUFFSIZE 128

static obj* parse_atom(const_expression e, size_t *num_parsed_p);
static obj* parse_list(const_expression e, size_t *num_parsed_p);
static obj* get_quote_list();
static bool contains_dot(const_expression e, size_t length);

static expression unparse_list(const obj *o);
static expression unparse_closure(const obj* o);
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
    list_of(o)->cdr = new_list_set(quoted, NULL);

  } else if (expr_start[0] == '(')  { // Expression starts with opening paren
    o = parse_list((char *) expr_start + 1, &expr_size);
    expr_size += 1; // for the opening parentheses character
    if (o == NULL) o = new_list();

  } else {
    o = parse_atom(expr_start, &expr_size);
  }

  if (num_parsed_p != NULL) *num_parsed_p = start + expr_size;
  return o;
}

expression unparse(const obj* o) {
  if (o == NULL) return NULL;

  if (is_atom(o) || is_number(o)) return unparse_atom(o);
  if (is_primitive(o)) return unparse_primitive(o);

  if (is_closure(o)) return unparse_closure(o);

  if (is_list(o)) {
    expression list_expr = unparse_list(o);
    if (list_expr == NULL) return strdup("()");

    expression e = malloc(1 + strlen(list_expr) + 2); // open, close, null
    if (e == NULL) return LOG_MALLOC_FAIL();
    e[0] = '(';
    strcpy((char *) e + 1, list_expr);
    strcpy((char *) e + 1 + strlen(list_expr), ")");
    free(list_expr);
    return e;
  }
  return NULL;
}

bool empty_expression(const_expression e) {
  if (e == NULL) return false;
  for (unsigned int i = 0; i < strlen(e); i++)
    if (!is_white_space(e[i])) return false;
  return true;
}

bool is_balanced(const_expression e) {
  int net = 0;
  for (size_t i = 0; i < strlen(e); i++) {
    if (e[i] == '(') net++;
    if (e[i] == ')') net--;
  }
  return net == 0;
}

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
 * Function: unparse_list
 * ----------------------
 * Turn a list into an expression that represents that list. Note: the produced lisp expression
 * will be in dynamically allocated space and will NOT contain opening and closing parentheses.
 * @param o: A lisp object that is a list to be unparsed
 * @return: A lisp expression that represents the passed lisp object
 */
static expression unparse_list(const obj *o) {
  if (o == NULL) return NULL;

  expression e;

  expression car_expr = unparse(list_of(o)->car);
  if (car_expr == NULL) return NULL;
  expression cdr_exp = unparse_list(list_of(o)->cdr);

  size_t car_size = strlen(car_expr);
  if (cdr_exp == NULL) {
    e = calloc(2 + car_size, 1);
    if (e == NULL) return NULL;
    strcpy(e, car_expr);

  } else {
    size_t cdr_size = strlen(cdr_exp);
    e = calloc(car_size + 1 + cdr_size + 1, 1);
    if (e == NULL) return NULL;

    strcpy(e, car_expr);
    strcpy((char*) e + car_size, " ");
    strcpy((char*) e + car_size + 1, cdr_exp);
    free(cdr_exp);
  }
  free(car_expr);
  return e;
}

/**
 * Function: unparse_closure
 * -------------------------
 * Serializes a closure object into a string
 * @param o: The closure object to serialize
 * @return: The serialization of the closure in a string
 */
static expression unparse_closure(const obj* o) {
  if (!is_closure(o)) return NULL;

  closure_t* closure = closure_of(o);
  expression para = unparse(closure->parameters);
  int num_capt = list_length(closure->captured);

  char buf[256];
  sprintf(buf, "<closure:%s, %d vars captured>", para, num_capt);
  free(para);
  return strdup(buf);
}

/**
 * Function: unparse_atom
 * ----------------------
 * Serializes an atom into a lisp expression in dynamically allocated memory.
 * This function will handle objects of type atom_obj as well as integer_obj and float_obj
 * @param o: Pointer to an atom object
 * @return: Pointer to dynamically allocated memory with the an expression representing the atom
 */
static expression unparse_atom(const obj *o) {
  if (o == NULL) return NULL;

  if (is_atom(o)) {
    atom_t atm = atom_of(o);
    expression e = malloc(strlen(atm) + 1); // Cant use "new_atom"
    if (e == NULL) return LOG_MALLOC_FAIL();
    return strcpy(e, atm);
  }

  if (is_int(o)) {
    expression e = calloc(BUFFSIZE, 1);
    sprintf(e, "%d", get_int(o));
    return e;
  }

  if (is_float(o)) {
    expression e = calloc(BUFFSIZE, 1);
    sprintf(e, "%g", get_float(o));
    return e;
  }
  return LOG_ERROR("Attempted to parse object that is not an atom");
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
  expression e = malloc(strlen(KMAG) + 2 + sizeof(void*) * 8 / 4 + strlen(RESET) + 1);
  if (e == NULL) return LOG_MALLOC_FAIL();
  void* p = NULL;
  memcpy(&p, (void**) primitive_of(o), sizeof(primitive_t));
  sprintf(e, KMAG "%p" RESET, p);
  return e;
}

/**
 * Function: parse_atom
 * --------------------
 * Parses an expression that represents an atom or number.
 * NOTE: If the expression can be turned into an integer or floating point object then it will be
 * and then the returned object will be of the integer_obj or float_obj instead of atom_obj. Also note
 * that integer object is preferred over float object (i.e. "3" will be parsed into an integer even
 * though it could also be parsed as a float)
 * @param e: A pointer to an atom expression
 * @param num_parsed_p: Pointer to a location to be populated with the number of characters parsed
 * @return: A lisp object representing the parsed atom in dynamically allocated memory
 */
static obj* parse_atom(const_expression e, size_t *num_parsed_p) {
  size_t size = atom_size(e);

  bool has_decimal = contains_dot(e, size);

  char* contents = strncpy(calloc(size + 1, 1), e, size);
  char* end;
  int int_value = (int) strtol(contents, &end, 0);
  bool is_integer = contents != end;

  float float_value = strtof(contents, &end);
  bool is_float = contents != end;

  obj* o;
  if (is_integer && !has_decimal) o = new_int(int_value);
  else if (is_float) o = new_float(float_value);
  else o = new_atom(contents);
  *num_parsed_p = size;
  free(contents);
  return o;
}

/**
 * Function: parse_list
 * --------------------
 * Parses an expression that represents a list. This expression should not start
 * with an opening parentheses. This function will parse until there is a closing parentheses
 * that closes the implicit opening parentheses. Note: this is NOT necessarily the first closing
 * parentheses as there may be lists nested inside of this list.
 * @param e: An expression representing a list
 * @param num_parsed_p: A pointer to a place where the number of parsed characters may be written. Must be valid
 * @return: Pointer to a lisp data structure object representing the lisp expression
 */
static obj* parse_list(const_expression e, size_t *num_parsed_p) {
  int start = distance_to_next_element(e);
  expression exprStart = (char*) e + start;

  if (exprStart[0] == ')') {
    *num_parsed_p = (size_t) start + 1;
    return NULL;
  } // Empty list or the end of a list

  size_t exprSize;
  obj* nextElement = parse_expression(exprStart, &exprSize); // will find closing paren
  obj* o = new_list_set(nextElement, NULL);

  size_t restSize;
  expression restOfList = (char*) exprStart + exprSize;
  list_of(o)->cdr = parse_list(restOfList, &restSize);

  *num_parsed_p = start + exprSize + restSize;
  return o;
}

/**
 * Function: get_quote_list
 * ------------------------
 * Creates a list where car points to a "quote" atom and cdr points to nothing
 * @return: Pointer to the list object
 */
static obj* get_quote_list() {
  size_t i;
  obj* quote_atom = parse_atom("quote", &i);
  if (quote_atom == NULL) return NULL;
  return new_list_set(quote_atom, NULL);
}

/**
 * Function: distance_to_next_element
 * ----------------------------------
 * Counts the number of characters of whitespace until a non-whitespace character is found
 * @param e: A lisp expression
 * @return: The number of characters of whitespace in the beginning
 */
static int distance_to_next_element(const_expression e) {
  unsigned long i;
  for (i = 0; i < strlen(e); i++)
    if (!is_white_space(e[i])) break;
  if (i == strlen(e)) return -1;
  return (int) i;
}

/**
 * Function: atom_size
 * -------------------
 * Finds the size of the atom pointed to in the expression
 * @param e: An expression that represents an atom
 * @return: The number of characters in that atom
 */
static size_t atom_size(const_expression e) {
  int i;
  for(i = 0; i < (int) strlen(e); i++) {
    if (is_white_space(e[i]) || e[i] == '(' || e[i] == ')') return (size_t) i;
  }
  return (size_t) i;
}

/**
 * Function: is_white_space
 * ------------------------
 * Checks if a single character is a whitespace character
 * @param character: The character to check
 * @return: True if that character is whitespace, false otherwise
 */
static const char* kWhitespace = " \t\n\r";
static bool is_white_space(char character) {
  return strchr(kWhitespace, character) != NULL;
}

/**
 * Function: contains_dot
 * ----------------------
 * Determines if an expression contains a decimal point/floating point
 * @param e: The expression to check for a decimal point in
 * @param length: The number of characters to check if there is a dot
 * @return: True if there is a decimal point in the expression, false otherwise.
 */
static bool contains_dot(const_expression e, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (e[i] == '.') return true;
  }
  return false;
}
