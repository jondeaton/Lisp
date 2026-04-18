/*
 * File: parser.c
 * --------------
 * Presents the implementation of the lisp parser
 */

#include <parser.h>
#include <lisp-objects.h>
#include <list.h>
#include <stack-trace.h>
#include <primitives.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>


#define KMAG  "\x1B[35m"
#define RESET "\033[0m"
#define BUFFSIZE 128

#define NIL_STR_REP "nil"

static obj* parse_atom(const_expression e, size_t *num_parsed_p);
static obj* parse_list(const_expression e, size_t *num_parsed_p);
static obj* get_quote_list(void);
static bool contains_dot(const_expression e, size_t length);

static expression unparse_list(const obj *o);
static expression unparse_closure(const obj* o);
static expression unparse_atom(const obj *o);
static expression unparse_primitive(const obj *o);

static size_t atom_size(const_expression e);
static bool is_white_space(char character);
static int distance_to_next_element(const_expression e);

obj* parse_expression(const_expression e, size_t *num_parsed_p) {
  assert(e != NULL);

  ssize_t start = distance_to_next_element(e);
  if (start == -1) {
    if (num_parsed_p != NULL) *num_parsed_p = strlen(e);
    return NULL;
  }
  expression expr_start = (char*) e + start;

  if (expr_start[0] == ')') {
    if (num_parsed_p != NULL) *num_parsed_p = (size_t) start + 1;
    return NULL;
  }

  obj* o;
  size_t expr_size;

  if (expr_start[0] == '\'') {
    o = get_quote_list();
    obj* quoted = parse_expression((char *) expr_start + 1, &expr_size);
    expr_size += 1;
    CDR(o) = new_list_set(quoted, NULL);

  } else if (expr_start[0] == '(')  {
    o = parse_list((char *) expr_start + 1, &expr_size);
    expr_size += 1;
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

  if (is_closure(o) || is_macro(o)) return unparse_closure(o);

  if (is_list(o)) {
    expression list_expr = unparse_list(o);
    if (list_expr == NULL)
      return strdup(NIL_STR_REP);

    expression e = malloc(1 + strlen(list_expr) + 2);
    MALLOC_CHECK(e);
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

static expression unparse_list(const obj *o) {
  if (o == NULL) return NULL;

  expression e;

  expression car_expr = unparse(CAR(o));
  if (car_expr == NULL) return NULL;
  expression cdr_exp = unparse_list(CDR(o));

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

static expression unparse_closure(const obj* o) {
  if (!is_closure(o) && !is_macro(o)) return NULL;

  expression para = unparse(PARAMETERS(o));
  const char *kind = is_macro(o) ? "macro" : "closure";

  char buf[256];
  sprintf(buf, "<%s:%s>", kind, para);
  free(para);
  return strdup(buf);
}

static expression unparse_atom(const obj *o) {
  if (o == NULL) return NULL;

  if (is_atom(o)) {
    return strdup(ATOM(o));
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
  LOG_ERROR("Attempted to parse object that is not an atom");
  return NULL;
}

static expression unparse_primitive(const obj *o) {
  if (o == NULL) return NULL;
  expression e = malloc(strlen(KMAG) + 2 + sizeof(void*) * 2 + strlen(RESET) + 1);
  MALLOC_CHECK(e);
  sprintf(e, KMAG "%p" RESET, (void*)(intptr_t)PRIMITIVE(o));
  return e;
}

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

static obj* parse_list(const_expression e, size_t *num_parsed_p) {
  int start = distance_to_next_element(e);
  expression exprStart = (char*) e + start;

  if (exprStart[0] == ')') {
    *num_parsed_p = (size_t) start + 1;
    return NULL;
  }

  size_t exprSize;
  obj* nextElement = parse_expression(exprStart, &exprSize);
  obj* o = new_list_set(nextElement, NULL);

  size_t restSize;
  expression restOfList = (char*) exprStart + exprSize;
  CDR(o) = parse_list(restOfList, &restSize);

  *num_parsed_p = start + exprSize + restSize;
  return o;
}

static obj* get_quote_list(void) {
  size_t i;
  obj* quote_atom = parse_atom("quote", &i);
  if (quote_atom == NULL) return NULL;
  return new_list_set(quote_atom, NULL);
}

static int distance_to_next_element(const_expression e) {
  unsigned long i;
  for (i = 0; i < strlen(e); i++)
    if (!is_white_space(e[i])) break;
  if (i == strlen(e)) return -1;
  return (int) i;
}

static size_t atom_size(const_expression e) {
  int i;
  for(i = 0; i < (int) strlen(e); i++) {
    if (is_white_space(e[i]) || e[i] == '(' || e[i] == ')') return (size_t) i;
  }
  return (size_t) i;
}

static const char* kWhitespace = " \t\n\r";
static bool is_white_space(char character) {
  return strchr(kWhitespace, character) != NULL;
}

static bool contains_dot(const_expression e, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (e[i] == '.') return true;
  }
  return false;
}
