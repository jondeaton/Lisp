/*
 * File: test.c
 * ------------
 * Lisp interpreter tester
 */

#include <lisp.h>
#include <parser.h>
#include <stdbool.h>

bool parserTest() {
  obj* o;
  expression_t e;

  o = parse("()", NULL);
  e = unparse(o);

  o = parse("(a)", NULL);
  e = unparse(o);

  o = parse("(a b c)", NULL);
  e = unparse(o);

  o = parse("(car (quote (a b c))", NULL);
  e = unparse(o);

  return true;
}

int main(int argc, char* argv[]) {
  parserTest();
  return 0;
}