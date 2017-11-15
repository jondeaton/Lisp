/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop for Lisp
 */

#include "repl.h"
#include "environment.h"
#include "evaluator.h"
#include <string.h>
#include <sys/file.h>

#define BUFSIZE 1024
#define PROMPT "> "
#define REPROMPT ">>"

// Static function declarations
static obj* read_expression(FILE *fd, bool prompt, bool* eof);
static expression get_expression(FILE *fd, bool prompt, bool* eof);
static void print_object(FILE *fd, const obj *o);
static void reprompt(const_expression expr);
static int get_indentation_size(const_expression expr);
static int get_net_balance(const_expression expr);
static void update_net_balance(char next_character, int* netp);

obj* env;

void repl_init() {
  env = init_env();
  init_allocated();
}

void repl_run_program(const char* program_file) {
  if (program_file == NULL) return;
  FILE* fd = fopen(program_file, O_RDONLY);

  bool eof = false;
  while (!eof) {
    obj* o = read_expression(fd, false, &eof);
    if (o == NULL) break; // Error -> end
    obj* evaluation = eval(o, env);
    print_object(stdout, evaluation);
    clear_allocated();
  }
}

void repl_run() {
  bool eof = false;
  while (!eof) {
    obj* o = read_expression(stdin, true, &eof);
    if (o == NULL) {
      fprintf(stderr, "Invalid expression\n");
      continue;
    };
    obj* evaluation = eval(o, env);
    print_object(stdout, evaluation);
    clear_allocated();
  }
}

expression repl_eval(const_expression expr) {
  if (expr == NULL) return NULL;

  obj* o = parse_expression(expr, NULL);
  obj* result_obj = eval(o, env);
  expression result = unparse(result_obj);
  clear_allocated();
  return result;
}

void repl_dispose() {
  dispose_allocated();
  dispose_recursive(env);
}

/**
 * Function: read_expression
 * -------------------------
 * Reads the next expression from standard input, turns it into a list object,
 * and then returns the object (in dynamically allocated memory
 * @param fd: The file descriptor to read the next expression from
 * @param prompt: If true, print prompt to standard output (for interactive prompt)
 * @return: The parsed lisp object from dynamically allocated memory
 */
static obj* read_expression(FILE *fd, bool prompt, bool* eof) {
  expression next_expr = get_expression(fd, prompt, eof);
  if (next_expr == NULL) return NULL;

  obj* o = parse_expression(next_expr, NULL);
  free(next_expr);
  return o;
};

/**
 * Function: get_expression
 * ------------------------
 * Read the next expression from a file descriptor (e.g. a file or standard input)
 * @return: The expression in a dynamically allocated memory location
 * @param fd: A file descriptor to read input from
 * @return: An expression that was read from that file descriptor
 */
static expression get_expression(FILE *fd, bool prompt, bool* eof) {
  if (prompt) printf(PROMPT);

  char buff[BUFSIZE];
  void* p = fgets(buff, sizeof buff, fd);
  *eof = p == NULL;
  size_t input_size = strlen(buff);

  size_t total_size = input_size;
  expression e = malloc(sizeof(char) * (input_size + 1));
  if (e == NULL) {
    fprintf(stderr, "Memory allocation failure.\n");
    return NULL;
  }

  strcpy(e, buff);

  while (true) {
    bool valid = is_valid(e);
    bool balanced = is_balanced(e);
    if (valid && balanced) return e;
    if (!valid || *eof) return NULL;

    if (prompt) reprompt(e);
    void* p = fgets(buff, sizeof buff, fd);
    *eof = p == NULL;

    input_size = strlen(buff);

    e = realloc(e, sizeof(char) * (total_size + input_size + 1));
    if (e == NULL) {
      fprintf(stderr, "Memory allocation failure.\n");
      return NULL;
    }

    strcpy((char*) e + total_size, buff);
    total_size += input_size;
  }
}

/**
 * Function: print_object
 * ----------------------
 * Serializes an object and prints it to a file
 * @param fd: File descriptor to print serialization to
 * @param o: The object to serialize and print
 */
static void print_object(FILE *fd, const obj *o) {
  if (fd == NULL) {
    fprintf(stderr, "Invalid file descriptor\n");
    return;
  }

  expression serialization = unparse(o);
  if (serialization) fprintf(fd, "null\n");
  else fprintf(fd, "%s\n", serialization);

  free(serialization);
};

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
  if (next_character == '(') (*netp)++;
  if (next_character == ')') (*netp)--;
}