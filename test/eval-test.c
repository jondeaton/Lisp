
#include "eval-test.h"
 
bool test_single_eval(const_expression expr, const_expression expected) {
  LispInterpreter* interpreter = interpreter_init();
  expression result_exp = interpret_expression(interpreter, expr);
  interpreter_dispose(interpreter);

  bool test_result;
  if (result_exp == NULL)
    test_result = expected == NULL;
  else
    test_result = result_exp && strcmp(result_exp, expected) == 0;

  if (verbose)
    printf("%s Evaluation:\t%s\n", test_result ? PASS : FAIL, expr);

  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);
  return test_result;
}

bool test_multi_eval(const_expression before[], const_expression expr, const_expression expected) {

  LispInterpreter* interpreter = interpreter_init();
  for (int i = 0; before[i]; i++) free(interpret_expression(interpreter, before[i]));
  expression result_exp = interpret_expression(interpreter, expr);
  interpreter_dispose(interpreter);

  bool test_result = result_exp && strcmp(result_exp, expected) == 0;

  if (verbose)
    printf("%s Multi eval:\t%s\n", test_result ? PASS : FAIL, expr);
  if (!test_result) {
    printf(KRED "\tExpecting:\t%s\n", expected);
    printf("\tResult:\t\t%s\n" RESET, result_exp);
  }
  free(result_exp);
  return test_result;
}


/**
 * Function: test_quote
 * --------------------
 * Tests if the quote language feature works correctly
 * @return: True if all the tests pass, false otherwise
 */
int test_quote() {
  printf(KMAG "\nTesting quote...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(quote hello)", "hello") ? 0 : 1;
  num_fails += test_single_eval("(quote (a b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("'hello", "hello") ? 0 : 1;
  num_fails += test_single_eval("'(a b c)", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("(quote)", NULL) ? 0 : 1;
  num_fails += test_single_eval("(quote 1 2)", NULL) ? 0 : 1;
  num_fails += test_single_eval("(quote wan too three)", NULL) ? 0 : 1;
  printf("Test quote: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_car_cdr
 * ----------------------
 * Tests for proper functionality of car and cdr, printing results to standard output
 * @return: True if all the tests passed, false otherwise
 */
int test_car_cdr() {
  printf(KMAG "\nTesting car/cdr...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(car '(a b c))", "a") ? 0 : 1;
  num_fails += test_single_eval("(cdr '(a b c))", "(b c)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cdr '(a b c d)))", "(c d)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cdr '(a b c d)))", "(c d)") ? 0 : 1;
  num_fails += test_single_eval("(cdr (car '('(a b c) d e f)))", "((a b c))") ? 0 : 1;
  printf("Test car/cdr: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_atom
 * -------------------
 * Tests the functionality of the atom primitive.
 * @return: True if all atom tests pass, false otherwise
 */
int test_atom() {
  printf(KMAG "\nTesting atom...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(atom 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom ())", "t") ? 0 : 1;
  num_fails += test_single_eval("(atom '(a b c))", "()") ? 0 : 1;
  printf("Test atom: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_eq
 * -----------------
 * Tests the functionality of the eq primitive
 * @return: The number of tests that were failed
 */
int test_eq() {
  printf(KMAG "\nTesting eq...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(eq 'a 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq 'a 'b)", "()") ? 0 : 1;
  num_fails += test_single_eval("(eq '() '())", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq (car '(a b c)) 'a)", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq 3 3)", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq 3.0 3.0)", "t") ? 0 : 1;
  num_fails += test_single_eval("(eq 3.0 3)", "()") ? 0 : 1;
  num_fails += test_single_eval("(eq 3 3.0)", "()") ? 0 : 1;
  num_fails += test_single_eval("(eq (cons 'a 'b) (cons 'a 'c))", "()") ? 0 : 1;
  num_fails += test_single_eval("(eq (cons 'a 'b) (cons 'a 'b))", "()") ? 0 : 1;
  printf("Test eq: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_cons
 * -------------------
 * Tests the functionality of the cons primitive
 * @return: The number of cons tests that failed
 */
int test_cons() {
  printf(KMAG "\nTesting cons...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(cons 'a '(b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("(cons 'a (cons 'b (cons 'c '())))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("(car (cons 'a '(b c)))", "a") ? 0 : 1;
  num_fails += test_single_eval("(cdr (cons 'a '(b c)))", "(b c)") ? 0 : 1;
  printf("Test cons: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_cond
 * -------------------
 * Tests the functionality of the cond primitive
 * @return: The number of cond tests that
 */
int test_cond() {
  printf(KMAG "\nTesting cond...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(cond ((eq 'a 'b) 'first) ((atom 'a) 'second))", "second") ? 0 : 1;
  num_fails += test_single_eval("(cond ((eq 'a 'b) 'first) ((atom '(a)) 'second) ((eq (car (cdr '(a b c))) 'b) (cdr '(x y z !))))", "(y z !)") ? 0 : 1;
  printf("Test cond: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_set
 * ------------------
 * Tests whether something can be set correctly in the environment
 * @return: The number of tests that were failed
 */
int test_set() {
  printf(KMAG "\nTesting set...\n" RESET);
  int num_fails = 0;

  // Testing that you can set something
  const_expression set_x[] = {
    "(set 'x 5)",
    NULL,
  };
  num_fails += test_multi_eval(set_x, "x", "5") ? 0 : 1;

  const_expression set_y[] = {
    "(set 'y 5)",
    "(set 'y 10)",
    NULL,
  };
  num_fails += test_multi_eval(set_y, "y", "10") ? 0 : 1;

  // Test that you can set something as the evaluation of another expression
  const_expression set_x_eval[] = {
    "(set 'x (eq (car '(a b c)) 'a))",
    NULL
  };
  num_fails += test_multi_eval(set_x_eval, "(cond (x '5) ('() '6))", "5") ? 0 : 1;

  printf("Test set: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_math
 * -------------------
 * Tests if arithmetic works correctly
 * @return: The number of tests that failed
 */
int test_math() {
  printf(KMAG "\nTesting math...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("(= 1 1)", "t") ? 0 : 1;
  num_fails += test_single_eval("(= 1 0)", "()") ? 0 : 1;
  num_fails += test_single_eval("(+ 1 1)", "2") ? 0 : 1;
  num_fails += test_single_eval("(+ 20 -25)", "-5") ? 0 : 1;
  num_fails += test_single_eval("(- 13 7)", "6") ? 0 : 1;
  num_fails += test_single_eval("(- 10 100)", "-90") ? 0 : 1;
  num_fails += test_single_eval("(* 1337 0)", "0") ? 0 : 1;
  num_fails += test_single_eval("(* 6 7)", "42") ? 0 : 1;
  num_fails += test_single_eval("(/ 42 6)", "7") ? 0 : 1;
  num_fails += test_single_eval("(/ 42 100)", "0") ? 0 : 1;

  const_expression set_x[] = {
    "(set 'x 5)",
    NULL
  };
  num_fails += test_multi_eval(set_x, "(+ x 5)", "10") ? 0 : 1;

  const_expression set_xy[] = {
    "(set 'x 7)",
    "(set 'y 13)",
    NULL
  };
  num_fails += test_multi_eval(set_xy, "(+ x y)", "20") ? 0 : 1;
  num_fails += test_multi_eval(set_xy, "(- x y)", "-6") ? 0 : 1;
  num_fails += test_multi_eval(set_xy, "(* x y)", "91") ? 0 : 1;
  num_fails += test_multi_eval(set_xy, "(/ y x)", "1") ? 0 : 1;
  num_fails += test_multi_eval(set_xy, "(% y x)", "6") ? 0 : 1;

  printf("Test math: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_lambda
 * ---------------------
 * Tests the functionality of the lambda function language feature
 * @return: The number of tests that failed
 */
int test_lambda() {
  printf(KMAG "\nTesting lambda...\n" RESET);
  int num_fails = 0;
  num_fails += test_single_eval("((lambda (x) (car x)) '(a b c))", "a") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x) (cdr x)) '(a b c))", "(b c)") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x y) (cons x (cdr y))) 'a '(z b c))", "(a b c)") ? 0 : 1;
  num_fails += test_single_eval("((lambda (x) (cons 'z x)) '(a b c))", "(z a b c)") ? 0 : 1;

  const_expression before0[] = {
    "(set 'y '(a b c))",
    "(set 'f  (lambda (x) (cons x y)))",
    NULL,
  };
  num_fails += test_multi_eval(before0, "(f '(1 2 3))", "((1 2 3) a b c)") ? 0 : 1;

  const_expression before1[] = {
    "(set 'y '(a b c))",
    "(set 'y '(4 5 6))",
    "(set 'f    (lambda (x) (cons x y)))",
    NULL,
  };
  num_fails += test_multi_eval(before1, "(f '(1 2 3))", "((1 2 3) 4 5 6)") ? 0 : 1;

  const_expression before2[] = {
    "(set 'caar (lambda (x) (car (car x))))",
    "(set 'f    (lambda (x) (cons 'z x)))",
    "(set 'g    (lambda (x) (f (caar x))))",
    NULL,
  };
  num_fails += test_multi_eval(before2,"(g '(((a b) c) d) )", "(z a b)") ? 0 : 1;

  const_expression before3[] = {
    "(set 'make-adder (lambda (x) (lambda (y) (+ x y))))",
    "(set 'add-5 (make-adder 5)",
    NULL,
  };

  num_fails += test_multi_eval(before3, "(add-5 7)", "12") ? 0 : 1;

  const_expression before4[] = {
    "(set 'make-prepender (lambda (x) (lambda (y) (cons x y))))",
    "(set 'prepend-z (make-prepender 'z))",
    NULL,
  };
  num_fails += test_multi_eval(before4, "(prepend-z '(a b c))", "(z a b c)") ? 0 : 1;

  const_expression before5[] = {
    "(set 'f (lambda () 4))",
    "(f)",
    NULL
  };

  num_fails += test_multi_eval(before5, "(f)", "4") ? 0 : 1;

  const_expression before6[] = {
    "(set 'f (lambda () (+ 5 6)))",
    "(f)",
    NULL
  };

  num_fails += test_multi_eval(before6, "(f)", "11") ? 0 : 1;

  printf("Test lambda: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_closure
 * ----------------------
 * Tests the functionality of creating closures with lambda functions
 * @return: The number of tests that failed
 */
int test_closure() {
  printf(KMAG "\nTesting closure...\n" RESET);
  int num_fails = 0;

  const_expression before[] = {
    "(set 'f (lambda (x y) (+ x y)))",
    "(set 'add-5 (f 5)",
    NULL
  };

  num_fails += test_multi_eval(before, "(add-5 100)", "105") ? 0 : 1;

  printf("Test closure: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_recursion
 * ------------------------
 * Tests the functionality of using recursion with set and lambda functions
 * @return: The number of tests that failed
 */
int test_recursion() {
  printf(KMAG "\nTesting recursion...\n" RESET);
  int num_fails = 0;

  // 5!
  const_expression five[] = {
    "(set 'factorial (lambda (x)  (cond ((= x 0) 1) ((= 1 1) (* x (factorial (- x 1)))))))",
    NULL,
  };
  num_fails += test_multi_eval(five, "(factorial 5)", "120") ? 0 : 1;

  // 8!
  const_expression eight[] = {
    "(set 'factorial (lambda (x)  (cond ((= x 0) 1) (t (* x (factorial (- x 1)))))))",
    NULL,
  };
  num_fails += test_multi_eval(eight, "(factorial 8)", "40320") ? 0 : 1;

  // ith element
  const_expression end[] = {
    "(set 'ith (lambda (x i) (cond ((= i 0) (car x)) ((= 1 1) (ith (cdr x) (- i 1))))))",
    NULL,
  };
  num_fails += test_multi_eval(end, "(ith '(1 2 3 4 5) 2)", "3") ? 0 : 1;

  const_expression repeat[] = {
    "(set 'repeat (lambda (item n) (cond ((= n 1) item) (t (cons (car item) (repeat item (- n 1)))))))",
    NULL,
  };
  num_fails += test_multi_eval(repeat, "(repeat '(3) 7)", "(3 3 3 3 3 3 3)") ? 0 : 1;

  printf("Test recursion: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}

/**
 * Function: test_Y_combinator
 * ---------------------------
 * Tests the functionality of purely functional recursion in the lisp interpreter
 * @return: The number of tests that failed
 */
int test_Y_combinator() {
  printf(KMAG "\nTesting Y Combinator...\n" RESET);
  int num_fails = 0;

  // Testing Y combinator with factorial function definition
//  const_expression before[] = {
//    "(set 'Y (lambda (H) ((lambda (x) (H (x x))) (lambda (x) (H (x x))))))",
//    "(set 'F (lambda (G) (lambda (x)  (cond ((= x 0) 1) ((= 1 1) (* x (G (- x 1))))))))",
//    NULL,
//  };
//  num_fails += test_multi_eval(before, "((Y F) 5)", "120") ? 0 : 1;
  printf("Test Y Combinator: %s\n", num_fails == 0 ? PASS : FAIL);
  return num_fails;
}