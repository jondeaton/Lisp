//
// Created by Jonathan Deaton on 3/29/18.
//

#include "test.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * Function: run_all_tests
 * -----------------------
 * Runs all tests and determines if the interpreter is passing all of the tests
 * @return: True if all of the tests passed, false otherwise
 */
int run_all_tests() {
  int num_fails = 0;
  num_fails += test_parser();
  num_fails += test_quote();
  num_fails += test_car_cdr();
  num_fails += test_atom();
  num_fails += test_eq();
  num_fails += test_cons();
  num_fails += test_cond();
  num_fails += test_set();
  num_fails += test_math();
  num_fails += test_lambda();
  num_fails += test_closure();
  num_fails += test_recursion();
  num_fails += test_Y_combinator();
  return num_fails;
}

int main() {
  int fd = open("/dev/null", O_WRONLY);
  dup2(fd, STDERR_FILENO); // redirect stderr --> /dev/null

  int num_fails = run_all_tests();
  if (num_fails == 0) printf(KGRN "All tests passed.\n");
  else printf(KRED "%d test(s) failed\n" RESET, num_fails);

  close(fd);
  return 0;
}