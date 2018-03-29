/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop for Lisp
 */

#include <interpreter.h>
#include <garbage-collector.h>
#include <environment.h>
#include <evaluator.h>
#include <stack-trace.h>
#include <list.h>

#include <string.h>
#include <sys/file.h>

#include <readline/readline.h>
#include <readline/history.h>

#define KBLU  "\x1B[34m"
#define RESET "\033[0m"
#define BUFSIZE 512
char buff[BUFSIZE];
#define PROMPT "> "
#define REPROMPT ">>"

// Static function declarations
static obj *read_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error);
static expression get_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error);
static void print_object(FILE *fd, const obj *o);
static expression get_expression_from_prompt(bool* eof);
static expression get_expression_from_file(FILE *fd, bool *eof, bool *syntax_error);
static expression reprompt(const_expression expr);
static int get_indentation_size(const_expression expr);
static int get_net_balance(const_expression expr);
static void update_net_balance(char next_character, int* netp);

struct LispInterpreterImpl {
  obj* env;  // Interpreter environment
  GarbageCollector* gc;
};

LispInterpreter* interpreter_init() {
  LispInterpreter* interp = malloc(sizeof(LispInterpreter));
  interp->env = init_env();
  interp->gc = gc_init();
  return interp;
}

void interpret_program(LispInterpreter *interpreter, const char *program_file, bool verbose) {
  if (!program_file) return; // no program to interpret
  FILE* fd = fopen(program_file, "r");

  bool eof = false;
  bool syntax_error = false;
  while (!eof) {
    obj* o = read_expression(fd, false, &eof, &syntax_error);
    if (o == NULL && syntax_error) {
      LOG_ERROR("Syntax error.");
      break;
    }
    obj* result = eval(o, &interpreter->env, interpreter->gc);
    if (result == NULL) {
      if (verbose) LOG_MSG("NULL");
      break;
    }
    if (verbose) print_object(stdout, result);
    gc_clear(interpreter->gc); // collect garbage
  }
  fclose(fd);
}

void interpret_fd(LispInterpreter *interpreter, FILE *fd_in, FILE *fd_out, bool verbose) {
  bool eof = false;
  while (!eof) {
    obj* o = read_expression(fd_in, true, &eof, NULL);
    if (eof) break;
    if (o == NULL) {
      LOG_ERROR("Invalid expression");
      continue;
    }
    obj* result = eval(o, &(interpreter->env), interpreter->gc);
    if (result == NULL && verbose) LOG_MSG("NULL");
    print_object(fd_out, result);
    gc_clear(interpreter->gc);
  }
}

expression interpret_expression(LispInterpreter *interpreter, const_expression expr) {
  if (expr == NULL) return NULL;

  obj* o = parse_expression(expr, NULL);
  if (o == NULL) return NULL;
  obj* result_obj = eval(o, &(interpreter->env), interpreter->gc);
  expression result = unparse(result_obj);
  gc_clear(interpreter->gc); // frees the objects in result_obj that were allocated during eval
  dispose_recursive(o);
  return result;
}

void interpreter_dispose(LispInterpreter *interpreter) {
  gc_dispose(interpreter->gc);
  dispose_recursive(interpreter->env);
  free(interpreter);
}

/**
 * Function: read_expression
 * -------------------------
 * Reads the next expression from standard input, turns it into a list object,
 * and then returns the object (in dynamically allocated memory)
 * @param fd: The file descriptor to read the next expression from
 * @param prompt: If true, print prompt to standard output (for interactive prompt)
 * @return: The parsed lisp object from dynamically allocated memory
 */
static obj *read_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error) {
  expression next_expr = get_expression(fd, prompt, eof, syntax_error);
  if (next_expr == NULL) return NULL;
  if (prompt) add_history(next_expr);
  obj* o = parse_expression(next_expr, NULL);
  free(next_expr);
  return o;
}

/**
 * Function: get_expression
 * ------------------------
 * Read the next expression from a file descriptor (e.g. a file or standard input)
 * @return: The expression in a dynamically allocated memory location
 * @param fd: A file descriptor to read input from
 * @return: An expression that was read from that file descriptor
 */
static expression get_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error) {
  if (prompt) return get_expression_from_prompt(eof);
  else return get_expression_from_file(fd, eof, syntax_error);
}

/**
 * Function: get_expression_from_prompt
 * ------------------------------------
 * Gets the next expression from an interactive prompt from standard input
 * @param eof: Pointer to a boolean to write either EOF was encountered
 * @return: The next expression entered on the interactive prompt
 */
static expression get_expression_from_prompt(bool* eof) {
  char* e = readline(PROMPT);
  *eof = e == NULL;
  if (e == NULL) return NULL;

  bool empty = empty_expression(e);
  if (empty) {
    free(e);
    return get_expression_from_prompt(eof);
  }

  size_t input_size = strlen(e);
  size_t total_size = input_size;

  while (true) {
    bool valid = is_valid(e);
    bool balanced = is_balanced(e);
    
    if (valid && balanced) return e;
    if (!valid || *eof) return NULL;

    char* line = reprompt(e);
    *eof = line == NULL;

    input_size = strlen(line);
    e = realloc(e, sizeof(char) * (total_size + 1 + input_size + 1));
    MALLOC_CHECK(e);

    strcpy(e + total_size, " ");
    strcpy(e + total_size + 1, line);
    free(line);
    total_size += 1 + input_size;
  }
}

/**
 * Function: get_expression_from_file
 * ----------------------------------
 * Retrieves the next expression in the file
 * @param fd: File descriptor to read from
 * @param eof: Pointer to a bool to write whether or not EOF occured
 * @return: The next expression read form the file descriptor
 */
static expression get_expression_from_file(FILE *fd, bool *eof, bool *syntax_error) {
  char* p = fgets(buff, sizeof buff, fd);
  *eof = p == NULL;
  size_t input_size = strlen(buff);

  size_t total_size = input_size;
  expression e = malloc(sizeof(char) * (input_size + 1));
  MALLOC_CHECK(e);

  strcpy(e, buff);

  while (true) {
    bool valid = is_valid(e);
    bool balanced = is_balanced(e);
    if (valid && balanced) return e;
    if (!valid) {
      if (syntax_error != NULL) *syntax_error = true;
      return NULL;
    }
    if (*eof) return NULL;

    p = fgets(buff, sizeof buff, fd);
    *eof = p == NULL;

    input_size = strlen(buff);

    e = realloc(e, sizeof(char) * (total_size + input_size + 1));
    MALLOC_CHECK(e);

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
    LOG_ERROR("Invalid file descriptor");
    return;
  }

  expression serialization = unparse(o);
  if (serialization) fprintf(fd, "%s\n", serialization);
  free(serialization);
}

/**
 * Function: reprompt
 * ------------------
 * Prints the properly indented re-prompt
 * @param expr: The expression to reprompt for
 */
static expression reprompt(const_expression expr) {

  // figure out the number of spaces to put for smart indentation
  int indentation = get_indentation_size(expr);

  // re-purpose the global buffer to store the prompt
  char space = ' ';
  strcpy(buff, REPROMPT);
  for (int i = 0; i < indentation; i++)
    memcpy((char*) buff + strlen(REPROMPT) + i, &space, sizeof(char));
  memset((char*) buff + strlen(REPROMPT) + indentation, 0, 1);

  return readline(buff);
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
  return get_net_balance(expr);
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
