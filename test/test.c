/*
 * File: test.c
 * ------------
 * Lisp interpreter tester
 */

#include "primitives.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"
#include "list.h"
#include <string.h>
#include <stdio.h>

// for colors
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
static int test_atom();
static int test_quote();

/**
 * Function: main
 * --------------
 * Entry point to the testing program
 * @param argc: Size of elements in argv
 * @param argv: Null terminated list of command line arguments
 * @return: Program exit code
 */
int main(int argc, char* argv[]) {
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
  return num_fails;
}

/**
 * Function: test_single_eval
 * --------------------------
 * Tests if a certain expression evaluated to the correct thing. Will print the
 * results to stdout
 * @param expr: The expression to evaluate
 * @param expected: The expected result of evaluating the expression
 * @return: True if the expressoin evluated to the expected thing, false otherwise
 */
static bool test_single_eval(const_expression expr, const_expression expected) {
  obj* env = initEnv(); // The global environment

  printf("Evaluating:\t%s\n", expr);
  printf("\tExpecting:\t%s\n", expected);
  obj* to_eval = parse_expression(expr, NULL);
  obj* eval_result = eval(to_eval, env);
  expression result_exp = unparse(eval_result);
  printf("\tResult:\t\t%s\n", result_exp);

  bool test_result = strcmp(result_exp, expected) == 0;

  // todo: dispose of unused things

  printf("\t\t%s\n", test_result ? "pass" : "fail");
  return test_result;
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

  printf("Parsing:\t%s\n", expr);
  printf("\tExpecting:\t%s\n", expected);

  obj* o = parse_expression(expr, NULL);
  expression result_exp = unparse(o);
  printf("\tResult:\t\t%s\n", result_exp);
  bool test_result = strcmp(result_exp, expected) == 0;
  printf("\t\t%s\n", test_result ? PASS : FAIL);
  return test_result;
}

/**
 * Function: parser_test
 * ---------------------
 * Tests the functionality of the parser and un-parser.
 * @return: The number of tests that failed
 */
static int test_parser() {
  printf("\nTesting parsing...\n");

  int num_fails = 0;

  num_fails += test_single_parse("atom!", "atom!") ? 0 : 1;
  num_fails += test_single_parse("()", "()") ? 0 : 1;
  num_fails += test_single_parse("(hello) there (jon)", "(hello)") ? 0 : 1;
  num_fails += test_single_parse("(a)", "(a)") ? 0 : 1;
  num_fails += test_single_parse("(a b c)", "(a b c)") ? 0 : 1;
  num_fails += test_single_parse("(test (a b c))", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("\t\t\r\n \t(test(a\tb\nc )\t\t\n \n\r    )      ", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("(quote (a b c d e f hello 123456789098))", "(quote (a b c d e f hello 123456789098))") ? 0 : 1;
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
  printf("\nTesting quote...\n");
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
  printf("\nTesting car/cdr...\n");
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
  printf("\nTesting atom...\n");
  int num_fails = 0;
  num_fails += test_single_eval("(atom 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom ())", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom (a b c))", "()") ? 0 : 1;
  printf("Test atom: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}
