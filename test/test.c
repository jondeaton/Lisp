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

static bool run_all_tests();
static bool test_eval(expression expr, expression expected);
static bool parser_test();
static bool test_car_cdr();
static bool test_atom();
static bool test_quote();

/**
 * Function: main
 * --------------
 * Entry point to the testing program
 * @param argc: Size of elements in argv
 * @param argv: Null terminated list of command line arguments
 * @return: Program exit code
 */
int main(int argc, char* argv[]) {
  bool all_passed = run_all_tests();
  if (all_passed) printf("All tests passed.\n");
  else printf("Some tests failed");
  return 0;
}

/**
 * Function: run_all_tests
 * -----------------------
 * Runs all tests and determines if the interpreter is passing all of the tests
 * @return: True if all of the tests passed, false otherwise
 */
static bool run_all_tests() {
  bool result = true;
  result &= parser_test();
  result &= test_quote();
  result &= test_car_cdr();
  //result &= test_atom();
  return result;
}

/**
 * Function: test_eval
 * -------------------
 * Tests if a certain expression evaluated to the correct thing. Will print the
 * results to stdout
 * @param expr: The expression to evaluate
 * @param expected: The expected result of evaluating the expression
 * @return: True if the expressoin evluated to the expected thing, false otherwise
 */
static bool test_eval(expression expr, expression expected) {
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
 * Function: test_quote
 * --------------------
 * Tests if the quote language feature works correctly
 * @return: True if all the tests pass, false otherwise
 */
static bool test_quote() {
  printf("Testing quote...\n");
  bool result = true;
  result &= test_eval("(quote hello)", "hello");
  result &= test_eval("(quote (a b c))", "(a b c)");
  result &= test_eval("'hello", "hello");
  result &= test_eval("'(a b c)", "(a b c)");
  printf("car/cdr: %s\n", result ? "pass" : "fail");
  return result;
}

/**
 * Function: test_car_cdr
 * ----------------------
 * Tests for proper functionality of car and cdr, printing results to standard output
 * @return: True if all the tests passed, false otherwise
 */
static bool test_car_cdr() {
  printf("Testing car and cdr...\n");
  bool result = true;
  result &= test_eval("(car '(a b c))", "a");
  result &= test_eval("(cdr '(a b c))", "(b c)");
  result &= test_eval("(cdr (cdr '(a b c d)))", "(c d)");
  result &= test_eval("(cdr (cdr '(a b c d)))", "(c d)");
  result &= test_eval("(cdr (car '('(a b c) d e f)))", "((a b c))");
  printf("car/cdr: %s\n", result ? "pass" : "fail");
  return result;
}

/**
 * Function: test_atom
 * -------------------
 * Tests the functionality of the atom primitive.
 * @return: True if all atom tests pass, false otherwise
 */
static bool test_atom() {
  printf("Testing atom...\n");
  bool result = true;
  result &= test_eval("atom 'a", "t");
  result &= test_eval("atom ()", "t");
  result &= test_eval("atom (a b c)", "()");
  return result;
}

/**
 * Function: parser_test
 * ---------------------
 * Tests the functionality of the parser and un-parser. This method simply prints
 * things to standard output, and does not automatically check if the proper behavior as expected.
 * @return: True if the program didn't crash, false otherwise
 */
static bool parser_test() {
  char* exprs[] = {"atom!",
                   "()",
                   "(hello) there (jon)",
                   "(a)",
                   "(a b c)",
                   "(test (a b c))",
                   "      (test(a b c )          )          ",
                   "(quote (a b c d e f hello 123456789098))",
                   "\'(a b c)",
                   "(car (quote (a b c)))",
                   "(car \'(a b c))",
                   NULL};

  for (int i = 0; exprs[i] != NULL; i++) {
    expression e = exprs[i];
    obj* o = parse_expression(e, NULL);
    expression expr = unparse(o);
    printf("%s => %s\n", (char*) e, (char*) expr);
  }

  return true;
}
