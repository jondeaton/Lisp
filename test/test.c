/*
 * File: test.c
 * ------------
 * Lisp interpreter tester
 */

#include "test.h"
#include <stdarg.h>
#include <string.h>

bool verbose = false;
#define TEST_NAME_BUFFER_LEN 128
#define TEST_UNKNOWN_NAME "unknown"

#define BOTH_NULL(a, b) ((a) == NULL) && ((b) == NULL)
#define RESULT(x) x ? PASS : FAIL

bool get_test_result(const char* expected, const char* result) {
  bool test_result;
  if (result && expected)
    test_result = strcmp(result, expected) == 0;
  else
    test_result = BOTH_NULL(result, expected);
  return test_result;
}

void print_single_result(const char* test_type,
                         const char* expression,
                         const char* expected,
                         const char* result,
                         bool test_result, const char *test_name_format, ...) {
  if (verbose)
    printf("%s %s:\t%s\n", RESULT(test_result), test_type, expression);

  if (!test_result) {
    char test_name[TEST_NAME_BUFFER_LEN];
    va_list vargs;
    va_start (vargs, test_name_format);
    int n = vsnprintf(test_name, TEST_NAME_BUFFER_LEN, test_name_format, vargs);
    printf(KRED "\nFailed: \"%s\"\n", n > 0 ? test_name : TEST_UNKNOWN_NAME);
    printf("\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result);
    va_end (vargs);
  }
}
