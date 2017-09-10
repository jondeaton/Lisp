/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop
 * for Lisp
 */

#include <repl.h>
#include <parser.h>
#include <environment.h>
#include <evaluator.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">> "

// Static function declarations
static expression_t getExpression(FILE* fd);

// Get expression_t from stdin, turn it into a list, return the list
obj* readExpression(FILE* fd, const char* prompt, const char* reprompt) {
  expression_t input = getExpression(fd);
  if (input == NULL) return NULL;

  size_t n;
  obj* o = parseExpression(input, &n);
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

  expression_t result = unparse(o);

  if (result) fprintf(fd, "%s", (char*) result);
  else perror("Error\n");
  free(result);
};

int repl() {
  obj* env = initEnv(); // The REPL global environment
  printf("%s\n", unparse(env));

  while (true) {
    obj* o = readExpression(stdin, PROMPT, REPROMPT);
    if (o == NULL) return errno;
    obj* evaluation = eval(o, env);
    print(stdout, evaluation);
  }
};

/**
 * Function: getExpression
 * -----------------------
 * Gets an expression from the user or file
 * @return : The expression in a dynamically allocated memory location
 */
static expression_t getExpression(FILE* fd) {
  printf(PROMPT);

  char buff[BUFSIZE];
  fgets(buff, sizeof buff, stdin);
  size_t inputSize = strlen(buff);

  size_t totalSize = inputSize;
  expression_t e = malloc(sizeof(char) * (inputSize + 1));
  if (e == NULL) return NULL;

  strcpy(e, buff);

  bool valid;
  while ((valid = isValid(e)) && !isBalanced(e)) {
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
