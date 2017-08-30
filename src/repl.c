/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop
 * for Lisp
 */

#include <tclDecls.h>
#include "repl.h"
#include "string.h"
#include "stdio.h"

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">> "

static expression getExpression();
static bool unbalancedExpression(expression e);
static obj* parse(expression input);
static expression unparse(obj* o);
static bool isWhiteSpace(char character);

// Get expression from stdin, turn it into a list, return the list
obj* read() {
  expression input = getExpression();
  obj* o = parse((expression) input);
  free(input);
  return o;
};

// Walk the list, if car is a primitive, apply it to cdr
obj* eval(obj* o) {

};

void print(obj* o) {
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

  while (unbalancedExpression(e)) {
    printf(REPROMPT);
    inputSize = scanf("%s", buff);
    e = realloc(e, (size_t) totalSize + inputSize);
    if (e == NULL) return NULL;

    strcpy(e + totalSize + 1, buff);
    totalSize += inputSize;
  }

  return e;
}

/**
 * Function: unbalancedExpression
 * ------------------------------
 * Checks if the expression has a balanced set of parentheses
 * @param e :
 * @return
 */
static bool unbalancedExpression(expression e) {
  size_t numOpen = 0;
  size_t numClose = 0;
  for (size_t i = 0; i < strlen(e), i++) {
    if (e[i] == '(') numOpen++;
    if (e[i] == ')') numClose++;
  }
  return numOpen > numClose;
}

/**
 * Function: parse
 * ---------------
 * Converts a lisp expression into a lisp data structure
 * @param input : The lisp expression to objectify
 * @return : Pointer to the lisp data structure
 */
static obj* parse(expression input) {
  size_t i = 0;
  int len = strlen(input);
  for (int i = 0; i < len; ++i) {

    if (isWhitespace(input[i])) continue;

    if (inpit[i] == ')') return list;

    if (input[i] == '(') {
      size_t listSize = findListSize(input + i); // num chars until closign paren
      list.car = parse(input + i + 1);
      parse(input + i + listSize + 1, list.cdr);
    } else {
      size_t tokenSize = findTokenSize(input); // this is the size of the next token

      char *copy = malloc(tokenSize);
      strcpy(input, copy, tokenSize);

      list.car = copy;
      list.cdr = parse(input + i + next_token_location + 1);
    }
  }
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