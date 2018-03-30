
#include "test.h"
#include "eval-test.h"

#include <string.h>

#define TEST_EVAL(e, expected) TEST_ITEM(test_single_eval, e, expected)
#define TEST_EVALS(pre, e, expected) TEST_ITEM(test_multi_eval, pre, e, expected)
#define TEST_ERROR(e) TEST_EVAL(e, NULL)

#define SERIES(name, ...) const_expression name[] = {__VA_ARGS__, NULL}

bool test_single_eval(const_expression expr, const_expression expected) {
  LispInterpreter* interpreter = interpreter_init();
  expression result = interpret_expression(interpreter, expr);
  interpreter_dispose(interpreter);

  bool test_result = get_test_result(expected, result);
  print_single_result("Evaluation", expr, expected, result, test_result);

  free(result);
  return test_result;
}

bool test_multi_eval(const_expression before[],
                     const_expression expr, const_expression expected) {

  LispInterpreter* interpreter = interpreter_init();
  for (int i = 0; before[i]; i++) free(interpret_expression(interpreter, before[i]));
  expression result_exp = interpret_expression(interpreter, expr);
  interpreter_dispose(interpreter);

  bool test_result = get_test_result(expected, result_exp);
  print_single_result("Series", expr, expected, result_exp, test_result);

  free(result_exp);
  return test_result;
}

DEF_TEST(quote) {
  TEST_INIT();

  TEST_EVAL("(quote hello)", "hello");
  TEST_EVAL("(quote (a b c))", "(a b c)");
  TEST_EVAL("'hello", "hello");
  TEST_EVAL("'(a b c)", "(a b c)");
  TEST_EVAL("'()", "()");
  TEST_EVAL("'t", "t");
  TEST_ERROR("(quote)");
  TEST_ERROR("(quote 1 2)");
  TEST_ERROR("(quote wan too three)");

  TEST_REPORT();
}

DEF_TEST(car_cdr) {
  TEST_INIT();
  TEST_EVAL("(car '(a b c))", "a");
  TEST_EVAL("(cdr '(a b c))", "(b c)");
  TEST_EVAL("(cdr (cdr '(a b c d)))", "(c d)");
  TEST_EVAL("(cdr (cdr '(a b c d)))", "(c d)");
  TEST_EVAL("(cdr (car '('(a b c) d e f)))", "((a b c))");
  TEST_ERROR("(car)"); // no argument
  TEST_ERROR("(cdr)");
  TEST_ERROR("(car 1)"); // argument is not a list
  TEST_ERROR("(cdr 1)");
  TEST_ERROR("(car '(wan) too)"); // too many arguments
  TEST_ERROR("(cdr '(wan) too)");
  TEST_REPORT();
}

DEF_TEST(atom) {
  TEST_INIT();

  TEST_EVAL("(atom 'a)", "t");
  TEST_EVAL("(atom ())", "t");
  TEST_EVAL("(atom '(a b c))", "()");
  TEST_ERROR("(atom)"); // no argument
  TEST_ERROR("(atom 1 2)"); // too many arguments

  TEST_REPORT();
}

DEF_TEST(eq) {
  TEST_INIT();
  TEST_EVAL("(eq 'a 'a)", "t");
  TEST_EVAL("(eq 'a 'b)", "()");
  TEST_EVAL("(eq '() '())", "t");
  TEST_EVAL("(eq (car '(a b c)) 'a)", "t");
  TEST_EVAL("(eq 3 3)", "t");
  TEST_EVAL("(eq 3.0 3.0)", "t");
  TEST_EVAL("(eq 3.0 3)", "()");
  TEST_EVAL("(eq 3 3.0)", "()");

  TEST_EVAL("(eq (cons 'x '(a b c)) '(x a b c))", "()");
  TEST_EVAL("(eq (cons 'x '(a b c)) '((x) a b c))", "()");

  TEST_ERROR("(eq)"); // no arguments
  TEST_ERROR("(eq one)"); // one argument
  TEST_ERROR("(eq one two three)"); // too many
  TEST_ERROR("(eq (cons 'a 'b) (cons 'a 'c))"); // no dot notation (yet) means
  TEST_ERROR("(eq (cons 'a 'b) (cons 'a 'b))"); // that these should error our

  TEST_REPORT();
}

DEF_TEST(cons) {
  TEST_INIT();

  TEST_EVAL("(cons 'a '(b c))", "(a b c)");
  TEST_EVAL("(cons 'a (cons 'b (cons 'c '())))", "(a b c)");
  TEST_EVAL("(car (cons 'a '(b c)))", "a");
  TEST_EVAL("(cdr (cons 'a '(b c)))", "(b c)");
  TEST_ERROR("(cons)"); // no arguments
  TEST_ERROR("(cons one)"); // one argument
  TEST_ERROR("(cons 'x '(a b c) three)"); // too many arguments
  TEST_ERROR("(cons 'x 'y)"); // second argument isn't a list (no dot notation yet)

  TEST_REPORT();
}

DEF_TEST(cond) {
  TEST_INIT();

  TEST_EVAL("(cond)", "()");
  TEST_EVAL("(cond (t 'a) (t 'b))", "a");
  TEST_EVAL("(cond ('t 1) (t 2))", "1");
  TEST_EVAL("(cond (() 'a) (t 'b))", "b");
  TEST_EVAL("(cond ('() 1) (t 2))", "2");
  TEST_EVAL("(cond ('() 1) (() 2))", "()");
  TEST_EVAL("(cond (t 'b) ())", "b"); // yeah this is kinda weird
  TEST_EVAL("(cond '())", "()"); // "quote" is not t
  TEST_EVAL("(cond ((eq 'a 'b) 'first) ((atom 'a) 'second))", "second");
  TEST_EVAL("(cond ((eq 'a 'b) 'first) ((atom '(a)) 'second) ((eq (car (cdr '(a b c))) 'b) (cdr '(x y z !))))", "(y z !)");

  TEST_ERROR("(cond ())"); // needs to have list of length two
  TEST_ERROR("(cond t)"); // not a list
  TEST_ERROR("(cond (t))");
  TEST_ERROR("(cond (()) ())");
  TEST_ERROR("(cond (() a) ())");
  TEST_ERROR("(cond (() a) (t))");
  TEST_ERROR("(cond () ())");

  TEST_REPORT();
}

DEF_TEST(set) {
  TEST_INIT();

  const_expression set_x[] = { // Testing that you can set something
    "(set 'x 5)",
    NULL,
  };
  TEST_EVALS(set_x, "x", "5");

  const_expression set_y[] = {
    "(set 'y 5)",
    "(set 'y 10)",
    NULL,
  };
  TEST_EVALS(set_y, "y", "10");

  // Test that you can set something as the evaluation of another expression
  const_expression set_x_eval[] = {
    "(set 'x (eq (car '(a b c)) 'a))",
    NULL
  };
  TEST_EVALS(set_x_eval, "(cond (x '5) ('() '6))", "5");

  TEST_ERROR("(set x)");
  TEST_ERROR("(set x y z)");
  TEST_ERROR("(set 1 4)"); // This shouldn't work...
  TEST_ERROR("(set t 4)");
  TEST_ERROR("(set 't 4)");
  TEST_ERROR("(set () 4)"); // This shouldn't work...
  TEST_ERROR("(set '() 4)"); // This shouldn't work...
  TEST_ERROR("(set 5 6)");
  TEST_ERROR("(set '(a) 4)");
  TEST_ERROR("(set '(a b c) 6)");
  TEST_ERROR("(set (lambda (x) x) 'e)");

  TEST_REPORT();
}

DEF_TEST(math) {
  TEST_INIT();

  TEST_EVAL("(= 1 1)", "t");
  TEST_EVAL("(= 1 0)", "()");
  TEST_EVAL("(+ 1 1)", "2");
  TEST_EVAL("(+ 20 -25)", "-5");
  TEST_EVAL("(- 13 7)", "6");
  TEST_EVAL("(- 10 100)", "-90");
  TEST_EVAL("(* 1337 0)", "0");
  TEST_EVAL("(* 6 7)", "42");
  TEST_EVAL("(/ 42 6)", "7");
  TEST_EVAL("(/ 42 100)", "0");

  SERIES(setx, "(set 'x 5)");
  TEST_EVALS(setx, "(+ x 5)", "10");

  SERIES(set_xy,
         "(set 'x 7)",
         "(set 'y 13)");
  TEST_EVALS(set_xy, "(+ x y)", "20");
  TEST_EVALS(set_xy, "(- x y)", "-6");
  TEST_EVALS(set_xy, "(* x y)", "91");
  TEST_EVALS(set_xy, "(/ y x)", "1");
  TEST_EVALS(set_xy, "(% y x)", "6");

  // weird ways to use these things
  TEST_EVAL("(+ 1 2 3 4)", "10");
  TEST_EVAL("(- 5)", "-5");
  TEST_EVAL("(* 1 2 3 4)", "24");

  // Too few arguments
  TEST_ERROR("(+)");
  TEST_ERROR("(-)");
  TEST_ERROR("(*)");
  TEST_ERROR("(/)");
  TEST_ERROR("(/ 3)");
  TEST_ERROR("(=)");
  TEST_ERROR("(= 4)");
  TEST_ERROR("(%)");
  TEST_ERROR("(% 3)");

  // Too many arguments
  TEST_ERROR("(- 3 4 5)");
  TEST_ERROR("(/ 3 4 5)");
  TEST_ERROR("(% 3 4 5)");

  TEST_REPORT();
}

DEF_TEST(lambda) {
  TEST_INIT();

  TEST_EVAL("((lambda (x) (car x)) '(a b c))", "a");
  TEST_EVAL("((lambda (x) (cdr x)) '(a b c))", "(b c)");
  TEST_EVAL("((lambda (x y) (cons x (cdr y))) 'a '(z b c))", "(a b c)");
  TEST_EVAL("((lambda (x) (cons 'z x)) '(a b c))", "(z a b c)");

  SERIES(zero,
         "(set 'y '(a b c))",
         "(set 'f  (lambda (x) (cons x y)))");
  TEST_EVALS(zero, "(f '(1 2 3))", "((1 2 3) a b c)");

  SERIES(one,
         "(set 'y '(a b c))",
         "(set 'y '(4 5 6))",
         "(set 'f    (lambda (x) (cons x y)))");
  TEST_EVALS(one, "(f '(1 2 3))", "((1 2 3) 4 5 6)");

  SERIES(two,
    "(set 'caar (lambda (x) (car (car x))))",
    "(set 'f    (lambda (x) (cons 'z x)))",
    "(set 'g    (lambda (x) (f (caar x))))");
  TEST_EVALS(two,"(g '(((a b) c) d) )", "(z a b)");

  SERIES(three,
         "(set 'make-adder (lambda (x) (lambda (y) (+ x y))))",
         "(set 'add-5 (make-adder 5)");
  TEST_EVALS(three, "(add-5 7)", "12");

  SERIES(four,
         "(set 'make-prepender (lambda (x) (lambda (y) (cons x y))))",
         "(set 'prepend-z (make-prepender 'z))");
  TEST_EVALS(four, "(prepend-z '(a b c))", "(z a b c)");

  SERIES(five,
         "(set 'f (lambda () 4))",
         "(f)");
  TEST_EVALS(five, "(f)", "4");

  SERIES(six,
         "(set 'f (lambda () (+ 5 6)))",
         "(f)");
  TEST_EVALS(six, "(f)", "11");

  TEST_ERROR("(lambda)");
  TEST_ERROR("(lambda 1)");
  TEST_ERROR("(lambda one two three four)");
  TEST_ERROR("(lambda t)");

  TEST_REPORT();
}

DEF_TEST(closure) {
  TEST_INIT();

  SERIES(one,
         "(set 'f (lambda (x y) (+ x y)))",
         "(set 'add-5 (f 5)");
  TEST_EVALS(one, "(add-5 100)", "105");

  TEST_REPORT();
}

DEF_TEST(recursion) {
  TEST_INIT();

  SERIES(factorial,
         "(set 'factorial (lambda (x) "
           "(cond "
           "((= x 0) 1) "
           "(t (* x (factorial (- x 1)))))))");
  TEST_EVALS(factorial, "(factorial 5)", "120");
  TEST_EVALS(factorial, "(factorial 8)", "40320");
  TEST_EVALS(factorial, "(factorial 0)", "0");

  // ith element
  SERIES(ith,
         "(set 'ith (lambda (x i)"
           "(cond"
           "((= i 0) (car x))"
           "(t (ith (cdr x) (- i 1))))))");
  TEST_EVALS(ith, "(ith '(1 2 3 4 5) 2)", "3");

  SERIES(repeat,
         "(set 'repeat (lambda (item n)"
           "(cond"
           "((= n 1) item)"
           "(t (cons (car item) (repeat item (- n 1)))))))");
  TEST_EVALS(repeat, "(repeat '(3) 7)", "(3 3 3 3 3 3 3)");

  TEST_REPORT();
}

DEF_TEST(Y_combinator) {
  TEST_INIT();

  // Testing Y combinator with factorial function definition
//  const_expression before[] = {
//    "(set 'Y (lambda (H) ((lambda (x) (H (x x))) (lambda (x) (H (x x))))))",
//    "(set 'F (lambda (G) (lambda (x)  (cond ((= x 0) 1) ((= 1 1) (* x (G (- x 1))))))))",
//    NULL,
//  };
//  TEST_EVALS(before, "((Y F) 5)", "120");

  TEST_REPORT();
}
