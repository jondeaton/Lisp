/*
 * File: test.c
 * ------------
 * Lisp interpreter tester
 */

#include <primitives.h>
#include <parser.h>
#include <environment.h>
#include <stdio.h>

static bool parserTest();
static bool envTest();

int main(int argc, char* argv[]) {
  envTest();
  parserTest();
  return 0;
}

static bool envTest() {
  return true;
}

static bool parserTest() {
  size_t n;

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
    expression_t e = exprs[i];
    obj* o = parseExpression(e, &n);
    expression_t expr = unparse(o);
    printf("%s => %s\n", (char*) e, (char*) expr);
  }

  return true;
}
