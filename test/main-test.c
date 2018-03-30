//
// Created by Jonathan Deaton on 3/29/18.
//

#include "test.h"

#include "eval-test.h"
#include "parse-test.h"

#include <fcntl.h>
#include <unistd.h>


/**
 * Function: run_all_tests
 * -----------------------
 * Runs all tests and determines if the interpreter is passing all of the tests
 * @return: True if all of the tests passed, false otherwise
 */
int run_all_tests() {
  int num_fails = 0, num_tests = 0;

  int nf, nt;
  RUN_TEST(parser);
  RUN_TEST(quote);
  RUN_TEST(car_cdr);
  RUN_TEST(atom);
  RUN_TEST(eq);
  RUN_TEST(cons);
  RUN_TEST(cond);
  RUN_TEST(set);
  RUN_TEST(math);
  RUN_TEST(lambda);
  RUN_TEST(closure);
  RUN_TEST(recursion);
  RUN_TEST(Y_combinator);

  return num_fails;
}

int main() {
  int fd = open("/dev/null", O_WRONLY);
  dup2(fd, STDERR_FILENO); // redirect stderr --> /dev/null

  verbose = false;

  int num_fails = run_all_tests();
  if (num_fails == 0) printf(KGRN "All tests passed.\n");
  else printf(KRED "%d test(s) failed\n" RESET, num_fails);

  close(fd);
  return 0;
}