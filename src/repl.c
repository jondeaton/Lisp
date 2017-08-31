/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop
 * for Lisp
 */

#include <tclDecls.h>
#include <lisp.h>
#include "repl.h"
#include "string.h"
#include "stdio.h"

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">> "

static expression getExpression();
static bool isBalanced(expression e);
static bool isValid(expression e);
static obj* parse(expression input);
static expression unparse(obj* o);
static bool isWhiteSpace(char character);

// Get expression from stdin, turn it into a list, return the list
obj* read() {
  expression input = getExpression();
  if (input == NULL) return NULL;
  obj* o = parse((expression) input);
  free(input);
  return o;
};

// Walk the list, if car is a primitive, apply it to cdr
obj* eval(obj* o) {
  if (o == NULL) return NULL;
  // todo...
};

void print(obj* o) {
  if (o == NULL) printf("Error");
  expression e = unparse(o);
  printf("%s", (char*) e);
};

void repl() {
  while (true) print(eval(read()));
};

/**
 * Function: getExpression
 * -----------------------
 * Gets an expression from the user
 * @return : The expression in a dynamically allocated location
 */
static expression getExpression() {
  printf(PROMPT);

  char buff[BUFSIZE];
  int inputSize = scanf("%s", buff);
  int totalSize = inputSize;
  expression e = malloc(sizeof(char) * (inputSize + 1));
  if (e == NULL) return NULL;

  strcpy(buff, e);

  bool valid;
  while ((valid = isValid(e)) && !isBalanced(e)) {
    printf(REPROMPT);
    inputSize = scanf("%s", buff);
    e = realloc(e, (size_t) totalSize + inputSize);
    if (e == NULL) return NULL;

    strcpy(e + totalSize + 1, buff);
    totalSize += inputSize;
  }
  if (!valid) return NULL;

  return e;
}

/**
 * Function: unbalanced
 * ------------------------------
 * Checks if all of the open parenthesis in the expression
 * are balanced by a closing parentheses.
 * @param e : A lisp expression
 * @return : True if there are at least as many
 */
static bool isBalanced(expression e) {
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
 * Checks if the expression is valid
 * @param e : The expression to check
 * @return : True if the expression is valid, false otherwise
 */
static bool isValid(expression e) {
  int net = 0;
  for (size_t i = 0; i < strlen(e); i++) {
    if (e[i] == '(') net++;
    if (e[i] == ')') net--;
    if (net < 0) return false;
  }
  return net == 0;
}

/**
 * Function: parse
 * ---------------
 * Converts a lisp expression into a lisp data structure
 * @param input : The lisp expression to objectify
 * @return : Pointer to the lisp data structure
 */
static obj* parse(expression e, size_t* numParsed) {

  // Check if its an atom
  if (e[0] != '(') {

  }

  size_t netOpen = 1;
  list* l = malloc(sizeof(list));

  size_t i = 1;
  while (i < strlen(e)) {
    if (isWhiteSpace(e[i])) continue;

    // Closing parenthesis
    if (netOpen == 0) {
      *numParsed == i;
      return l;
    }

    if (e[i] == '(') {
      netOpen++;


      size_t listSize = findListSize(e + i);
      list.car = parse(e + i + 1);
      parse(e + i + listSize + 1, list.cdr);
    } else {
      size_t tokenSize = findTokenSize(e); // this is the size of the next token

      char *copy = malloc(tokenSize);
      strcpy(e, copy, tokenSize);

      list.car = copy;
      list.cdr = parse(e + i + next_token_location + 1);
    }

    i++;
  }
  // Got to the end of the string;
  *numParsed = i - 1;
  return o;
}

/**
 * Function: unparse
 * -----------------
 * Takes a lisp object and turns it into its string
 * representation.
 * @param o : A lisp object
 * @return : An expression that represents the lisp data structure
 */
static expression unparse(obj* o) {
  // todo
}

/**
 * Function: isWhiteSpace
 * ----------------------
 * Checks if a single character is a whitespace character
 * @param character : The character to check
 * @return : True if that character is whitespace, false otherwise
 */
static const char kWhitespace[] = {' ', '\n', '\t', NULL};
static bool isWhiteSpace(char character) {
  size_t i = 0;
  while (kWhitespace[i] != NULL) {
    if (character == kWhitespace[i]) return true;
    i++;
  }
  return false;
}