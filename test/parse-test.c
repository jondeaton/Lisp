
#include <parser.h>
#include <list.h>
#include "parse-test.h"
#include "test.h"

#include <string.h>

#define TEST_PARSE(a, b) TEST_ITEM(test_single_parse, a, b)

bool test_single_parse(const_expression expr, const_expression expected) {
  obj* o = parse_expression(expr, NULL);
  expression result_exp = unparse(o);
  dispose_recursive(o);
  bool test_result = strcmp(result_exp, expected) == 0;

  if (verbose)
    printf("%s Parsing:\t%s\n", test_result ? PASS : FAIL, expr);
  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);
  return test_result;
}

DEF_TEST(parser) {
  TEST_INIT();

  TEST_PARSE("atom!", "atom!");
  TEST_PARSE("()", "()");
  TEST_PARSE("(hello) there (jon)", "(hello)");
  TEST_PARSE("(a)", "(a)");
  TEST_PARSE("(a b c)", "(a b c)");
  TEST_PARSE("(test (a b c))", "(test (a b c))");
  TEST_PARSE("123", "123");
  TEST_PARSE("(42)", "(42)");
  TEST_PARSE("(1 2 3 2701)", "(1 2 3 2701)");
  TEST_PARSE("3.14", "3.14");
  TEST_PARSE("(6.28)", "(6.28)");
  TEST_PARSE("(6.28 1.234 5 6)", "(6.28 1.234 5 6)");
  TEST_PARSE("\t\t\r\n \t(test(a\tb\nc )\t\t\n \n\r    )      ", "(test (a b c))");
  TEST_PARSE("(quote (a b c d e f hello 1234))", "(quote (a b c d e f hello 1234))");
  TEST_PARSE("((((((()))))))", "((((((()))))))");
  TEST_PARSE("'(a b c)", "(quote (a b c))");
  TEST_PARSE("(car (quote (a b c)))", "(car (quote (a b c)))");
  TEST_PARSE("(car '(a b c))", "(car (quote (a b c)))");
  TEST_PARSE("(atom 'a)", "(atom (quote a))");

  TEST_REPORT();
}
