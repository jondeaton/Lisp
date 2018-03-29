//
// Created by Jonathan Deaton on 3/29/18.
//


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

int test_parser() {
  printf(KMAG "\nTesting parsing...\n" RESET);

  int num_fails = 0;

  num_fails += test_single_parse("atom!", "atom!") ? 0 : 1;
  num_fails += test_single_parse("()", "()") ? 0 : 1;
  num_fails += test_single_parse("(hello) there (jon)", "(hello)") ? 0 : 1;
  num_fails += test_single_parse("(a)", "(a)") ? 0 : 1;
  num_fails += test_single_parse("(a b c)", "(a b c)") ? 0 : 1;
  num_fails += test_single_parse("(test (a b c))", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("123", "123") ? 0 : 1;
  num_fails += test_single_parse("(42)", "(42)") ? 0 : 1;
  num_fails += test_single_parse("(1 2 3 2701)", "(1 2 3 2701)") ? 0 : 1;
  num_fails += test_single_parse("3.14", "3.14") ? 0 : 1;
  num_fails += test_single_parse("(6.28)", "(6.28)") ? 0 : 1;
  num_fails += test_single_parse("(6.28 1.234 5 6)", "(6.28 1.234 5 6)") ? 0 : 1;
  num_fails += test_single_parse("\t\t\r\n \t(test(a\tb\nc )\t\t\n \n\r    )      ", "(test (a b c))") ? 0 : 1;
  num_fails += test_single_parse("(quote (a b c d e f hello 1234))", "(quote (a b c d e f hello 1234))") ? 0 : 1;
  num_fails += test_single_parse("((((((()))))))", "((((((()))))))") ? 0 : 1;
  num_fails += test_single_parse("'(a b c)", "(quote (a b c))") ? 0 : 1;
  num_fails += test_single_parse("(car (quote (a b c)))", "(car (quote (a b c)))") ? 0 : 1;
  num_fails += test_single_parse("(car '(a b c))", "(car (quote (a b c)))") ? 0 : 1;
  num_fails += test_single_parse("(atom 'a)", "(atom (quote a))") ? 0 : 1;

  printf("Test parsing: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}