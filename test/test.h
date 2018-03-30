//
// Created by Jonathan Deaton on 3/29/18.
//

#ifndef LISP_TEST_H
#define LISP_TEST_H

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

#define _TEST_NAME ((char*)__func__ + 5)
#define TEST_INIT() printf(KMAG "Testing %*s..." RESET, 13, _TEST_NAME); *num_tests = 0, *num_fails = 0
#define TEST_ITEM(f, ...) *num_fails += f(__VA_ARGS__) ? 0 : 1; (*num_tests)++
#define TEST_REPORT() printf(" %*d /%*d%*s\n", 3, (*num_tests - *num_fails),3, *num_tests, 20, *(num_fails) == 0 ? PASS : FAIL);

// Define and run a new test
#define DEF_TEST(name) void test_ ## name(int *num_tests, int *num_fails)
#define RUN_TEST(name) test_ ## name(&nt, &nf); num_fails += nf; num_tests += nt

extern bool verbose;

/**
 * Function: get_test_result
 * -------------------------
 * Gets the test result for a given expected and result string pair
 * @param expected: The expression that was expected
 * @param result: The expression that was the result of the test
 * @return: True if they are identical strings or both NULL
 */
bool get_test_result(const char* expected, const char* result);

/**
 * Function: print_single_result
 * -----------------------------
 * Prints the test result for a single test
 * @param test_type: The name of the type of the test
 * @param expression: The expression that was tested
 * @param expected: The expected result of the test
 * @param result: The expression that was the result
 * @param test_result: True if the test succeeded, false if failure.
 */
void print_single_result(const char* test_type,
                         const char* expression,
                         const char* expected,
                         const char* result,
                         bool test_result);

#endif //LISP_TEST_H
