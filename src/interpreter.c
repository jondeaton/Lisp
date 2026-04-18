/*
 * File: interpreter.c
 * -------------------
 * Presents the implementation of the Read-Eval-Print Loop for Lisp
 */

#include <interpreter.h>
#include <garbage-collector.h>
#include <environment.h>
#include <evaluator.h>
#include <stack-trace.h>
#include <list.h>
#include <assert.h>

#include <string.h>
#include <sys/file.h>

#include <readline/readline.h>
#include <readline/history.h>

#define KBLU  "\x1B[34m"
#define RESET "\033[0m"
#define BUFSIZE 512
char buff[BUFSIZE];
#define PROMPT "> "
#define REPROMPT "  "

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

static void strip_line_comment(char *s) {
  for (int i = 0; s[i]; i++) {
    if (s[i] == ';') { s[i] = '\0'; return; }
  }
}

bool interpreter_init(LispInterpreter *interpreter) {
  assert(interpreter != NULL);

  bool success = gc_init(&interpreter->gc);
  if (!success) return false;

  interpreter->env = init_env();
  if (interpreter->env == NULL) {
    gc_dispose(&interpreter->gc);
    return false;
  }

  // Track all environment objects in the GC
  gc_add_recursive(&interpreter->gc, interpreter->env);

  // Load the Lisp prelude
  #ifdef PRELUDE_PATH
  interpret_program(interpreter, PRELUDE_PATH, false);
  #endif

  return true;
}

void interpret_program(LispInterpreter *interpreter, const char *program_file, bool verbose) {
  if (!program_file) return;
  FILE* fd = fopen(program_file, "r");

  bool eof = false;
  bool syntax_error = false;
  while (!eof) {
    obj* o = read_expression(fd, false, &eof, &syntax_error);
    if (syntax_error) {
      LOG_ERROR("Syntax error.");
      break;
    }
    if (o == NULL) continue;
    gc_add_recursive(&interpreter->gc, o);
    obj* result = eval(o, interpreter);
    if (result == NULL) {
      if (verbose) LOG_MSG("NULL");
      break;
    }
    if (verbose) print_object(stdout, result);
    collect_garbage(&interpreter->gc, interpreter->env);
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
    gc_add_recursive(&interpreter->gc, o);
    obj* result = eval(o, interpreter);
    if (result == NULL && verbose) LOG_MSG("NULL");
    print_object(fd_out, result);
    collect_garbage(&interpreter->gc, interpreter->env);
  }
}

expression interpret_expression(LispInterpreter *interpreter, const_expression expr) {
  assert(interpreter != NULL);

  if (expr == NULL) return NULL;

  obj* o = PARSE(expr);
  if (o == NULL) {
    LOG_ERROR("Error parsing expression: %s", expr);
    return NULL;
  }

  gc_add_recursive(&interpreter->gc, o);
  obj* result_obj = eval(o, interpreter);
  expression result = unparse(result_obj);
  collect_garbage(&interpreter->gc, interpreter->env);
  return result;
}

void interpreter_dispose(LispInterpreter *interpreter) {
  gc_dispose(&interpreter->gc);
}

static obj *read_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error) {
  expression next_expr = get_expression(fd, prompt, eof, syntax_error);
  if (next_expr == NULL) return NULL;
  if (prompt) add_history(next_expr);
  obj* o = PARSE(next_expr);
  free(next_expr);
  return o;
}

static expression get_expression(FILE *fd, bool prompt, bool *eof, bool *syntax_error) {
  if (prompt) return get_expression_from_prompt(eof);
  else return get_expression_from_file(fd, eof, syntax_error);
}

static expression get_expression_from_prompt(bool* eof) {
  char* e = readline(PROMPT);
  *eof = e == NULL;
  if (e == NULL) return NULL;
  strip_line_comment(e);

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

static expression get_expression_from_file(FILE *fd, bool *eof, bool *syntax_error) {
  char* p = fgets(buff, sizeof buff, fd);
  *eof = p == NULL;
  if (*eof) return NULL;
  strip_line_comment(buff);
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
    if (!*eof) strip_line_comment(buff);

    input_size = strlen(buff);

    e = realloc(e, sizeof(char) * (total_size + input_size + 1));
    MALLOC_CHECK(e);

    strcpy((char*) e + total_size, buff);
    total_size += input_size;
  }
}

static void print_object(FILE *fd, const obj *o) {
  if (fd == NULL) {
    LOG_ERROR("Invalid file descriptor");
    return;
  }

  expression serialization = unparse(o);
  if (serialization) fprintf(fd, "%s\n", serialization);
  free(serialization);
}

static expression reprompt(const_expression expr) {
  int indentation = get_indentation_size(expr);

  char space = ' ';
  strcpy(buff, REPROMPT);
  for (int i = 0; i < indentation; i++)
    memcpy((char*) buff + strlen(REPROMPT) + i, &space, sizeof(char));
  memset((char*) buff + strlen(REPROMPT) + indentation, 0, 1);

  return readline(buff);
}

static int get_indentation_size(const_expression expr) {
  return get_net_balance(expr);
}

static int get_net_balance(const_expression expr) {
  int net = 0;
  for (size_t i = 0; i < strlen(expr); i++) {
    update_net_balance(expr[i], &net);
    if (net < 0) return false;
  }
  return net;
}

static void update_net_balance(char next_character, int* netp) {
  if (next_character == '(') (*netp)++;
  if (next_character == ')') (*netp)--;
}
