/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop for Lisp
 */

#include "repl.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"
#include <string.h>
#include <errno.h>

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">> "

// Static function declarations
static expression get_expression(FILE *fd);

// Get expression from stdin, turn it into a list, return the list
obj* read_expression(FILE *fd, const char *prompt, const char *reprompt) {
  expression input = get_expression(fd);
  if (input == NULL) return NULL;

  size_t n;
  obj* o = parse_expression(input, &n);
  unparse(o);

  free(input);
  return o;
};

// Stringifies the lisp data structure, prints it to stdout
void print(FILE* fd, obj* o) {
  if (o == NULL || fd == NULL) {
    perror("Error\n");
    return;
  }

  expression string_representation = unparse(o);

  if (string_representation == NULL) perror("Error\n");
  else fprintf(fd, "%s\n", (char*) string_representation);

  free(string_representation);
};

int repl() {
  obj* env = init_env(); // The REPL global environment
  init_allocated();
  while (true) {
    obj* o = read_expression(stdin, PROMPT, REPROMPT);
    if (o == NULL) break;
    obj* evaluation = eval(o, env);
    print(stdout, evaluation);
    clear_allocated();
  }
  dispose_allocated();
  dispose(env);
  return errno;
};

/**
 * Function: get_expression
 * ------------------------
 * Gets an expression from the user or file
 * @return: The expression in a dynamically allocated memory location
 */
static expression get_expression(FILE *fd) {
  printf(PROMPT);

  char buff[BUFSIZE];
  fgets(buff, sizeof buff, stdin);
  size_t inputSize = strlen(buff);

  size_t totalSize = inputSize;
  expression e = malloc(sizeof(char) * (inputSize + 1));
  if (e == NULL) return NULL;

  strcpy(e, buff);

  bool valid;
  while ((valid = is_valid(e)) && !is_balanced(e)) {
    printf(REPROMPT);
    scanf("%s", buff);
    inputSize = strlen(buff);

    e = realloc(e, sizeof(char) * (totalSize + inputSize + 1));
    if (e == NULL) return NULL;

    strcpy(e + totalSize, buff);
    totalSize += inputSize;
  }
  if (!valid) return NULL;
  return e;
}
