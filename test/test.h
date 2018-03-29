//
// Created by Jonathan Deaton on 3/29/18.
//

#ifndef LISP_TEST_H
#define LISP_TEST_H

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

#define TESTING(x) printf(KMAG "\nTesting %s...\n" RESET, x); int num_fails = 0, num_tests = 0
#define TEST_ITEM(f, ...) num_fails += f(__VA_ARGS__)  ? 0 : 1; num_tests++
#define REPORT(x, nf) printf("Test %s: %s\n", x, (nf) == 0 ? PASS : FAIL); return num_fails

extern bool verbose;

#endif //LISP_TEST_H
