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
#include <sys/file.h>

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">>"

// Static function declarations
static void reprompt(const_expression expr);
static expression get_expression(FILE *fd);
static int get_indentation_size(const_expression expr);
static int get_net_balance(const_expression expr);
static void update_net_balance(char next_character, int* netp);

void repl_run_program(const char* program_file) {
  if (program_file == NULL) return;
  FILE* fd = fopen(program_file, O_RDONLY);

}



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
  dispose_recursive(env);
  return errno;
};

/**
 * Function: get_expression
 * ------------------------
 * Read the next expression from a file descriptor (e.g. a file or standard input)
 * @return: The expression in a dynamically allocated memory location
 * @param fd: A file descriptor to read input from
 * @return: An expression that was read from that file descriptor
 */
static expression get_expression(FILE *fd, bool prompt) {
  if (prompt) printf(PROMPT);

  char buff[BUFSIZE];
  fgets(buff, sizeof buff, stdin);
  size_t inputSize = strlen(buff);

  size_t totalSize = inputSize;
  expression e = malloc(sizeof(char) * (inputSize + 1));
  if (e == NULL) return NULL;

  strcpy(e, buff);

  bool valid;
  while ((valid = is_valid(e)) && !is_balanced(e)) {
    if (prompt) reprompt(e);
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

/**
 * Function: reprompt
 * ------------------
 * Prints the properly indented re-prompt
 * @param expr: The expression to reprompt for
 */
static void reprompt(const_expression expr) {
  printf(REPROMPT);
  int indentation = get_indentation_size(expr);
  for (int i = 0; i < indentation; i++) printf(" ");
}

/**
 * Function: get_indentation_size
 * ------------------------------
 * Gets the number of spaces that should be put on the reprompt
 * so that things are aligned correctly.
 * @param expr: The expression to get the indentation for
 * @return: The number of spaces that should be added to the next REPL prompt line
 * so that it is properly indented.
 */
static int get_indentation_size(const_expression expr) {
  int total_net = get_net_balance(expr);
  int net = 0;
  int i = 0;
  for (i = 0; i < strlen(expr); i++) {
    if (net == total_net) return i;
    update_net_balance(expr[i], &net);
  }
  return i;
}

/**
 * Function: get_net_balance
 * -------------------------
 * Gets the number of open parenthesis that were not closed in an expression
 * @param expr: The expression to count parenthesis balance in
 * @return: The number of parenthesis in the expression that were opened but not closed
 */
static int get_net_balance(const_expression expr) {
  int net = 0;
  for (size_t i = 0; i < strlen(expr); i++) {
    update_net_balance(expr[i], &net);
    if (net < 0) return false;
  }
  return net;
}

/**
 * Function: update_net_balance
 * ----------------------------
 * Updates the net parenthesis balance to include a certain character
 * @param next_character: The next character in the expression
 * @param netp: Pointer to the place where the net open parenthesis count is stored
 */
static void update_net_balance(char next_character, int* netp) {
  if (next_character == '(') *netp++;
  if (next_character == ')') *netp--;
}