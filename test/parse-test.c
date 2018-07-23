
#include <parser.h>
#include <list.h>
#include "parse-test.h"
#include "test.h"

#include <stdarg.h>
#include <string.h>

#define TEST_PARSE(a, b, ...) TEST_ITEM(test_single_parse, a, b, __VA_ARGS__)

bool test_single_parse(const_expression expr, const_expression expected,
                       const char *test_name_format, ...) {

  obj* o = PARSE(expr);
  expression result = unparse(o);
  dispose_recursive(o);

  // compare result to expectation
  bool test_result = get_test_result(expected, result);

  // report the result
  va_list vargs;
  va_start (vargs, test_name_format);
  print_single_result("Parse", expr, expected, result, test_result,
                      test_name_format, vargs);
  va_end(vargs);

  free(result);
  return test_result;
}

DEF_TEST(parser) {
  TEST_INIT();

  TEST_PARSE("atom!", "atom!",                            "simple atom");
  TEST_PARSE("()", NIL_STR,                               "nil");
  TEST_PARSE("(hello) there (jon)", "(hello)",            "multiple expressions");
  TEST_PARSE("(a)", "(a)",                                "one element list");
  TEST_PARSE("(a b)", "(a b)",                            "two element list");
  TEST_PARSE("(a b c)", "(a b c)",                        "three element list");
  TEST_PARSE("(test (a b c))", "(test (a b c))",          "nested lists");
  TEST_PARSE("123", "123",                                "numeric");
  TEST_PARSE("(42)", "(42)",                              "numeric in list");
  TEST_PARSE("(1 2 3 2701)", "(1 2 3 2701)",              "multiple numerics in list");
  TEST_PARSE("3.14", "3.14",                              "floating point");
  TEST_PARSE("(6.28)", "(6.28)",                          "floating point in list");
  TEST_PARSE("(6.28 1.234 5 6)", "(6.28 1.234 5 6)",      "multiple floats in list");
  TEST_PARSE("\t\t\r\n \t(test(a\tb\nc )\t\t\n \n\r    )      ",
             "(test (a b c))",                            "spaces, \\t, \\n, \\r");
  TEST_PARSE("(quote a)", "(quote a)",                    "simple quote");
  TEST_PARSE("(quote (a b c d e f hello 1234))",
             "(quote (a b c d e f hello 1234))",          "quote of long list");
  TEST_PARSE("((((((()))))))", "((((((nil))))))",         "many nested lists");
  TEST_PARSE("'(a b c)", "(quote (a b c))",               "quote character");
  TEST_PARSE("(car (quote (a b c)))",
             "(car (quote (a b c)))",                     "nested quote");
  TEST_PARSE("(car '(a b c))", "(car (quote (a b c)))",   "nested quote character");
  TEST_PARSE("(atom 'a)", "(atom (quote a))",             "nested quote again");

  TEST_REPORT();
}
