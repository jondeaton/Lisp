/*
 * File: test.c
 * ------------
 * Lisp interpreter tester
 */

#include "primitives.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"
#include "repl.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Terminal colors
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

#define PASS "\x1B[32mPASS\033[0m"
#define FAIL "\x1B[31mFAIL\033[0m"

static int run_all_tests();
static bool test_single_eval(const_expression expr, const_expression expected);
static bool test_single_parse(const_expression expr, const_expression expected);
static int test_parser();
static int test_car_cdr();
static int test_quote();
static int test_atom();
static int test_eq();
static int test_cons();
static int test_cond();
static int test_set();
static int test_lambda();
static int test_math();
static int test_Y_combinator();
static int test_recursion();

/**
 * Function: main
 * --------------
 * Entry point to the testing program
 * @return: Program exit code
 */
int main() {
  int num_fails = run_all_tests();
  if (num_fails == 0) printf(KGRN "All tests passed.\n");
  else printf(KRED "%d test(s) failed\n" RESET, num_fails);
  return 0;
}

/**
 * Function: run_all_tests
 * -----------------------
 * Runs all tests and determines if the interpreter is passing all of the tests
 * @return: True if all of the tests passed, false otherwise
 */
static int run_all_tests() {
  int num_fails = 0;
  num_fails += test_parser();
  num_fails += test_quote();
  num_fails += test_car_cdr();
  num_fails += test_atom();
  num_fails += test_eq();
  num_fails += test_cons();
  num_fails += test_cond();
  num_fails += test_set();
  return num_fails;
  num_fails += test_lambda();
  num_fails += test_math();
  num_fails += test_Y_combinator();
  num_fails += test_recursion();
  return num_fails;
}

/**
 * Function: test_single_parse
 * ---------------------------
 * Tests to see if a single parsing works correctly
 * @param expr: The expression to parse and then unparse
 * @param expected: The expected result of parsing and then un-parsing the expression
 * @return: True if the result of parsing and un-parsing the expression is equal to the expected expression
 */
static bool test_single_parse(const_expression expr, const_expression expected) {
  obj* o = parse_expression(expr, NULL);
  expression result_exp = unparse(o);
  dispose_recursive(o);
  bool test_result = strcmp(result_exp, expected) == 0;

  printf("%s Parsing:\t%s\n", test_result ? PASS : FAIL, expr);
  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);
  return test_result;
}

/**
 * Function: test_single_eval
 * --------------------------
 * Tests if a certain expression evaluated to the correct thing. Will print the
 * results to stdout
 * @param expr: The expression to evaluate
 * @param expected: The expected result of evaluating the expression
 * @return: True if the expression evaluated to the expected thing, false otherwise
 */
static bool test_single_eval(const_expression expr, const_expression expected) {
  repl_init();
  expression result_exp = repl_eval(expr);
  repl_dispose();

  bool test_result = strcmp(result_exp, expected) == 0;

  printf("%s Evaluation:\t%s\n", test_result ? PASS : FAIL, expr);

  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);
  return test_result;
}

/**
 * Function: test_multi_eval
 * -------------------------
 * Tests if a single expression evaluates to the correct thing after first executing a series of
 * expressions beforehand
 * @param before: NULL terminated list of expressions to evaluate in the same environment beforehand
 * @param expr: The expression to evaluate after the list of expression, and verify the output of
 * @param expected: The expected output of evaluating expr
 * @return: True if expr evaluates to expected, and false otherwise
 */
static bool test_multi_eval(const_expression before[], const_expression expr, const_expression expected) {

  repl_init();
  for (int i = 0; before[i]; i++) free(repl_eval(before[i]));
  expression result_exp = repl_eval(expr);
  repl_dispose();
  bool test_result = strcmp(result_exp, expected) == 0;

  printf("%s Multi eval:\t%s\n", test_result ? PASS : FAIL, expr);

  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);

  return test_result;
}

/**
 * Function: parser_test
 * ---------------------
 * Tests the functionality of the parser and un-parser.
 * @return: The number of tests that failed
 */
static int test_parser() {
  printf(KMAG "\nTesting parsing...\n" RESET);

  int num_fails = 0;

  num_fails += test_single_parse("atom!", "atom!") ? 0 : 1;
  num_fails += test_single_parse("()", "()") ? 0 : 1;
  num_fails += test_single_parse("(hello) there (jon)", "(hello)") ? 0 : 1;
  num_fails += test_single_parse("(a)", "(a)") ? 0 : 1;
  num_fails += test_single_parse("(a b c)", "(a b c)") ? 0 : 1;
  num_fails += test_single_parse("(test (a b c))", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("\t\t\r\n \t(test(a\tb\nc )\t\t\n \n\r    )      ", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("(quote (a b c d e f hello 123456789098))", "(quote (a b c d e f hello 123456789098))") ? 0 : 1;
  num_fails += test_single_parse("((((((()))))))", "((((((()))))))") ? 0 : 1;
  num_fails += test_single_parse("'(a b c)", "(quote (a b c))") ? 0 : 1;
  num_fails += test_single_parse("(car (quote (a b c)))", "(car (quote (a b c)))") ? 0 : 1;
  num_fails += test_single_parse("(car '(a b c))", "(car (quote (a b c)))") ? 0 : 1;
  num_fails += test_single_parse("(atom 'a)", "(atom (quote a))") ? 0 : 1;

  printf("Test parsing: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_quote
 * --------------------
 * Tests if the quote language feature works correctly
 * @return: True if all the tests pass, false otherwise
 */
static int test_quote() {
  printf(KMAG "\nTesting quote...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(quote hello)", "hello") ? 0 : 1;
  num_fails += test_single_eval("(quote (a b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("'hello", "hello") ? 0 : 1;
  num_fails += test_single_eval("'(a b c)", "(a b c)") ? 0 : 1;
  printf("Test quote: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_car_cdr
 * ----------------------
 * Tests for proper functionality of car and cdr, printing results to standard output
 * @return: True if all the tests passed, false otherwise
 */
static int test_car_cdr() {
  printf(KMAG "\nTesting car/cdr...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(car '(a b c))", "a") ? 0 : 1;
  num_fails += test_single_eval("(cdr '(a b c))", "(b c)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cdr '(a b c d)))", "(c d)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cdr '(a b c d)))", "(c d)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (car '('(a b c) d e f)))", "((a b c))") ? 0 : 1;
  printf("Test car/cdr: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_atom
 * -------------------
 * Tests the functionality of the atom primitive.
 * @return: True if all atom tests pass, false otherwise
 */
static int test_atom() {
  printf(KMAG "\nTesting atom...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(atom 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom ())", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom '(a b c))", "()") ? 0 : 1;
  printf("Test atom: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_eq
 * -----------------
 * Tests the functionality of the eq primitive
 * @return: The number of tests that were failed
 */
static int test_eq() {
  printf(KMAG "\nTesting eq...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(eq 'a 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq 'a 'b)", "()") ? 0 : 1;
  num_fails += test_single_eval("(eq '() '())", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq (car '(a b c)) 'a)", "t") ? 0 : 1;
  printf("Test eq: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_cons
 * -------------------
 * Tests the functionality of the cons primitive
 * @return: The number of cons tests that failed
 */
static int test_cons() {
  printf(KMAG "\nTesting cons...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(cons 'a '(b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("(cons 'a (cons 'b (cons 'c '())))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("(car (cons 'a '(b c)))", "a") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cons 'a '(b c)))", "(b c)") ? 0 : 1;
  printf("Test cons: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_cond
 * -------------------
 * Tests the functionality of the cond primitive
 * @return: The number of cond tests that
 */
static int test_cond() {
  printf(KMAG "\nTesting cond...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(cond ((eq 'a 'b) 'first) ((atom 'a) 'second))", "second") ? 0 : 1;
  num_fails += test_single_eval("(cond ((eq 'a 'b) 'first) ((atom '(a)) 'second) ((eq (car (cdr '(a b c))) 'b) (cdr '(x y z !))))", "(y z !)") ? 0 : 1;
  printf("Test cond: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_set
 * ------------------
 * Tests whether something can be set correctly in the environment
 * @return: The number of tests that were failed
 */
static int test_set() {
  printf(KMAG "\nTesting set...\n" RESET);
  int num_fails = 0;

  // Testing that you can set something
  const_expression set_x[] = {
    "(set 'x '5)",
    NULL,
  };
  num_fails += test_multi_eval(set_x, "x", "5") ? 0 : 1;

  return num_fails;

  const_expression set_y[] = {
    "(set 'y '10)",
    NULL,
  };
  num_fails += test_multi_eval(set_y, "y", "10") ? 0 : 1;

  // Test that you can set something as the evaluation of another expression
  const_expression set_x_eval[] = {
    "(set 'x (eq (car '(a b c)) 'a))",
    NULL
  };
  num_fails += test_multi_eval(set_x_eval, "(cond (x '5) ('() '6))", "5") ? 0 : 1;

  // Dynamic scoping test
  const_expression before[] = {
    "(set 'y '(a b c))",
    "(set 'f    (lambda (x) (cons x y)))",
    NULL,
  };
  num_fails += test_multi_eval(before, "(f '(1 2 3))", "((1 2 3) a b c)") ? 0 : 1;

  // Dynamic scoping test
  const_expression before2[] = {
    "(set 'y '(a b c))",
    "(set 'y '(4 5 6))",
    "(set 'f    (lambda (x) (cons x y)))",
    NULL,
  };
  num_fails += test_multi_eval(before2, "(f '(1 2 3))", "((1 2 3) 4 5 6)") ? 0 : 1;

  printf("Test set: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_lambda
 * ---------------------
 * Tests the functionality of the lambda function language feature
 * @return: The number of tests that failed
 */
static int test_lambda() {
  printf(KMAG "\nTesting lambda...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("((lambda (x) (car x)) '(a b c))", "a") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x) (cdr x)) '(a b c))", "(b c)") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x y) (cons x (cdr y))) 'a '(z b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x) (cons 'z x)) '(a b c))", "(z a b c)") ? 0 : 1;

  const_expression before[] = {
    "(set 'caar (lambda (x) (car (car x))))",
    "(set 'f    (lambda (x) (cons 'z x)))",
    "(set 'g    (lambda (x) (f (caar x))))",
    NULL,
  };
  num_fails += test_multi_eval(before,"(g '(((a b) c) d) )", "(z a b)") ? 0 : 1;

  const_expression before1[] = {
    "(set 'make-prepender (lambda (x) (lambda (y) (cons x y))))",
    "(set 'prepend-z (make-prepender 'z))",
    NULL,
  };
  num_fails += test_multi_eval(before1, "(prepend-z '(a b c))", "(z a b c)") ? 0 : 1;

  printf("Test lambda: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_math
 * -------------------
 * Tests if arithmetic works correctly
 * @return: The number of tests that failed
 */
static int test_math() {
  printf(KMAG "\nTesting math...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(= 1 1)", "t") ? 0 : 1;
  num_fails += test_single_eval("(= 1 0)", "()") ? 0 : 1;
  num_fails += test_single_eval("(+ 1 1)", "2") ? 0 : 1;
  num_fails += test_single_eval("(+ 20 -25)", "-5") ? 0 : 1;
  num_fails += test_single_eval("(- 13 7)", "6") ? 0 : 1;
  num_fails += test_single_eval("(- 10 100)", "-90") ? 0 : 1;
  num_fails += test_single_eval("(* 1337 0)", "0") ? 0 : 1;
  num_fails += test_single_eval("(* 6 7)", "42") ? 0 : 1;
  num_fails += test_single_eval("(/ 42 6)", "7") ? 0 : 1;
  num_fails += test_single_eval("(/ 42 100)", "0") ? 0 : 1;
  printf("Test math: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_Y_combinator
 * ---------------------------
 * Tests the functionality of purely functional recursion in the lisp interpreter
 * @return: The number of tests that failed
 */
static int test_Y_combinator() {
  printf(KMAG "Testing Y Combinator...\n" RESET);
  int num_fails = 0;

  // Testing Y combinator with factorial function definition
  const_expression before[] = {
    "(set 'Y (lambda (f) ((lambda (x) (f (x x))) (lambda (x) (f (x x))))))"
      "(set 'F (lambda (f) (lambda (n) (if (= n 0) 1 (* n (f (- n 1)))))))"
  };
  num_fails += test_multi_eval(before, "((Y F) 5)", "120") ? 0 : 1;
  printf("Test Y Combinator: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

static int test_recursion() {
  printf(KMAG "Testing recursion...\n" RESET);
  // todo: implement some recursion tests
  printf("No tests yet.\n");
  int num_fails = 0;
  printf("Test recursion: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}
