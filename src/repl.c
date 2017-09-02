/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop
 * for Lisp
 */

#include <repl.h>
#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">> "

// Static function declarations
static expression_t getExpression(FILE* fd);

// Get expression_t from stdin, turn it into a list, return the list
obj* read(FILE* fd, const char const* prompt, const char const* reprompt) {
  expression_t input = getExpression(fd);
  if (input == NULL) return NULL;
  obj* o = parse((expression_t) input, NULL);
  free(input);
  return o;
};

// Stringifies the lisp data structure, prints it to stdout
void print(FILE* fd, obj* o) {
  if (o == NULL) {
    perror("Error\n");
    return;
  }

  expression_t result = unparse(o);

  if (result) fprintf(fd, "%s", (char*) result);
  else perror("Error\n");
  free(result);
};

// Self explanatory
int repl() {
  while (true) {
    obj* o = read(stdin, PROMPT, REPROMPT);
    if (o == NULL) return errno;
    obj* evaluation = eval(o);
    print(stdout, evaluation);
  }
};

// Static functions

/**
 * Function: getExpression
 * -----------------------
 * Gets an expression from the user
 * @return : The expression in a dynamically allocated location
 */
static expression_t getExpression(FILE* fd) {
  printf(PROMPT);

  char buff[BUFSIZE];
  int inputSize = fscanf(fd, "%s", buff);
  int totalSize = inputSize;
  expression_t e = malloc(sizeof(char) * (inputSize + 1));
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
