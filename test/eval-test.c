
#include "test.h"
#include "eval-test.h"
#include "parser.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#define TEST_EVAL(e, expected, ...) TEST_ITEM(test_single_eval, e, expected, __VA_ARGS__)
#define TEST_EVALS(pre, e, expected, ...) TEST_ITEM(test_multi_eval, pre, e, expected, __VA_ARGS__)
#define TEST_ERROR(e, ...) TEST_EVAL(e, NULL, __VA_ARGS__)
#define TEST_TRUE(e, ...) TEST_EVAL(e, "t", __VA_ARGS__)
#define TEST_FALSE(e, ...) TEST_EVAL(e, NIL_STR, __VA_ARGS__)

#define TEST_EXPR_SIZE 128
#define TEST_RESULT_SIZE 128

// Macro for easy creation of a series of expressions to evaluate
#define SERIES(name, ...) const_expression name[] = {__VA_ARGS__, NULL}

/**
 * Function: test_single_eval
 * --------------------------
 * Tests the evaluation of a single expression. This function will
 * create a new Lisp interpreter, evaluate an expression in that interpreter
 * and compare the result against the expected result.
 * @param expr The expression to evaluate of in the lisp interpreter
 * @param expected The expected result of evaluating the test expression
 * @param test_name_format printf-style format specifier for the test name
 * @param ... Variable length arguments for formatting of the test name
 * @return True if the result of evaluating the expression matches the expected
 * expression exactly, and false otherwise.
 */
bool test_single_eval(const_expression expr, const_expression expected,
                      const char *test_name_format, ...) {

  // Get the result: create a new lisp interpreter to evaluate expression in
  LispInterpreter interpreter;
  bool success = interpreter_init(&interpreter);
  if (!success) return false;

  expression result = interpret_expression(&interpreter, expr);
  interpreter_dispose(&interpreter);

  // compare result with expectation
  bool test_result = get_test_result(expected, result);

  // report the result
  va_list vargs;
  va_start (vargs, test_name_format);
  print_single_result("Evaluation", expr, expected, result, test_result,
                      test_name_format, vargs);
  va_end(vargs);

  free(result);
  return test_result;
}

/**
 * Function: test_multi_eval
 * -------------------------
 * Tests the evaluation of a single expression after first having
 * executed a set of expressions which put the interpreter environment
 * into state under which the final "test" expression needs to be evaluated in.
 * The result of evaluating the final expression is compares against the expected
 * result and
 * @param setup_expressions NULL terminated list of expressions to evaluate
 * before evaluating the final test expression
 * @param test_expression The final expression to evaluate, the result of which
 * will be compared to determine the result of the test
 * @param expected Expression which the test expression should evaluate to
 * @param test_name_format printf-style format specifier for the test name
 * @param ... Variable length arguments for formatting the name of the test
 * @return True if the result of the test expression is identical to the
 * provided expected expression.
 */
bool test_multi_eval(const_expression setup_expressions[],
                     const_expression test_expression, const_expression expected,
                     const char *test_name_format, ...) {

  assert(setup_expressions != NULL);
  assert(test_expression != NULL);
  assert(expected != NULL);

  // Create a lisp interpreter to evaluate the list of expressions
  LispInterpreter interpreter;
  bool success = interpreter_init(&interpreter);
  if (!success) return false;

  // Loop through set-up expressions, evaluating each
  for (int i = 0; setup_expressions[i] != NULL; i++) {
    expression result = interpret_expression(&interpreter, setup_expressions[i]);

    // It's fine if some of the expressions result in errors. It may be of interest
    // to see if the interpreter will evaluate something correctly even after
    // having had an error in a previous expression.
    if (result == NULL) continue;
    free(result);
  }

  expression result_exp = interpret_expression(&interpreter, test_expression);
  interpreter_dispose(&interpreter);

  // compare the final result with expectation
  bool test_result = get_test_result(expected, result_exp);

  // report the result
  va_list vargs;
  va_start (vargs, test_name_format);
  print_single_result("Series", test_expression, expected, result_exp, test_result,
                      test_name_format, vargs);
  va_end(vargs);

  free(result_exp);
  return test_result;
}

DEF_TEST(quote) {
  TEST_INIT();

  TEST_EVAL("(quote hello)", "hello",      "explicit quote");
  TEST_EVAL("(quote (a b c))", "(a b c)",  "explicit quote 2");
  TEST_EVAL("'hello", "hello",             "quote atom");
  TEST_EVAL("'(a b c)", "(a b c)",         "quote list");
  TEST_EVAL("'()", NIL_STR,                "quote nil");
  TEST_EVAL("'t", "t",                     "quote truth");

  TEST_ERROR("(quote)",                    "no arguments");
  TEST_ERROR("(quote 1 2)",                "too many arguments");
  TEST_ERROR("(quote wan too three)",      "way too many arguments");

  TEST_REPORT();
}

DEF_TEST(car_cdr) {
  TEST_INIT();

  TEST_EVAL("(car '(a b c))", "a",                  "car a list");
  TEST_EVAL("(cdr '(a b c))", "(b c)",              "cdr a list");
  TEST_EVAL("(cdr (cdr '(a b c d)))", "(c d)",      "nested cdr");
  TEST_EVAL("(cdr (car '('(a b c) d e f)))",
            "((a b c))",                            "car inside cdr");

  TEST_EVAL("(car ())", NIL_STR,                    "car nil gives nil");
  TEST_EVAL("(cdr ())", NIL_STR,                    "cdr nil gives nil");
  TEST_EVAL("(cdr '(W))", NIL_STR,                  "cdr size 1 gives nil");

  TEST_ERROR("(car)",                               "car no arguments");
  TEST_ERROR("(cdr)",                               "cdr no arguments");
  TEST_ERROR("(car 1)",                             "car argument not a list");
  TEST_ERROR("(cdr 1)",                             "cdr argument not a list");
  TEST_ERROR("(car '(wan) too)",                    "car too many arguments");
  TEST_ERROR("(cdr '(wan) too)",                    "cdr too many arguments");

  TEST_REPORT();
}

DEF_TEST(atom) {
  TEST_INIT();

  TEST_EVAL("(atom 'a)", "t",                       "simple atom");
  TEST_EVAL("(atom ())", "t",                       "nil is atom");
  TEST_EVAL("(atom '(a b c))", NIL_STR,             "non-empty list isn't atom");

  TEST_ERROR("(atom)",                              "no arguments");
  TEST_ERROR("(atom 1 2)",                          "too many arguments");
  TEST_ERROR("(atom 1 2 3 4 5!!!)",                 "way too many arguments");

  TEST_REPORT();
}

DEF_TEST(eq) {
  TEST_INIT();

  TEST_TRUE("(eq 'a 'a)",                            "atom with same name");
  TEST_FALSE("(eq 'a 'b)",                           "atoms with differnet names");
  TEST_TRUE("(eq '() '())",                          "nil equals nil");
  TEST_FALSE("(eq '(a) '(a)",                        "lists aren't equal");

  TEST_TRUE("(eq (car '(a b c)) 'a)",                "atom from evaluation");
  TEST_TRUE("(eq 3 3)",                              "integer equals integer");
  TEST_TRUE("(eq 3.0 3.0)",                          "float equals float");
  TEST_FALSE("(eq 3.0 3)",                           "float not equal integer");
  TEST_FALSE("(eq 3 3.0)",                           "integer not equal float");
  TEST_FALSE("(eq (cons 'x '(a b c)) '(x a b c))",   "two identical lists aren't equal");
  TEST_FALSE("(eq (cons 'x '(a b c)) '((x) a b c))", "two dissimilar lists aren't equal");

  TEST_ERROR("(eq)",                                  "no arguments");
  TEST_ERROR("(eq one)",                              "one argument");
  TEST_ERROR("(eq one two three)",                    "three arguments (too many)");
  TEST_ERROR("(eq (cons 'a 'b) (cons 'a 'c))",        "no dot notation");
  TEST_ERROR("(eq (cons 'a 'b) (cons 'a 'b))",        "no dot notation 2");

  TEST_REPORT();
}

DEF_TEST(cons) {
  TEST_INIT();

  TEST_EVAL("(cons 'a '(b c))", "(a b c)",            "simple cons atom");
  TEST_EVAL("(cons 'a (cons 'b (cons 'c '())))",
            "(a b c)",                                "nested cons");
  TEST_EVAL("(car (cons 'a '(b c)))", "a",            "car of cons");
  TEST_EVAL("(cdr (cons 'a '(b c)))", "(b c)",        "cdr of cons");

  TEST_EVAL("(cons () '(a b c)", "(nil a b c)",       "cons a nil");
  TEST_EVAL("(cons 'a ())", "(a)",                    "cons to nil");
  TEST_EVAL("(cons 'a (cdr '(a)))", "(a)",            "cons to nil from eval");
  TEST_EVAL("(cons () ())", "(nil)",                  "cons nil nil");

  SERIES(one,
         "(set 'x '(a b c)");
  TEST_EVALS(one, "(cons x x))", "((a b c) a b c)",            "cons with self");

  TEST_ERROR("(cons)",                               "no arguments error");
  TEST_ERROR("(cons one)",                           "one argument error");
  TEST_ERROR("(cons 'x '(a b c) three)",             "too many arguments");
  TEST_ERROR("(cons 'x 'y)",                         "second argument isn't a list");

  TEST_REPORT();
}

DEF_TEST(cond) {
  TEST_INIT();

  TEST_EVAL("(cond)", NIL_STR,                       "cond no arguments");
  TEST_EVAL("(cond (t 'a) (t 'b))", "a",             "simple cond with truth");
  TEST_EVAL("(cond ('t 1) (t 2))", "1",              "cond with quoted truth");
  TEST_EVAL("(cond (() 'a) (t 'b))", "b",            "cond second is true");

  TEST_EVAL("(cond ('() 1) (t 2))", "2",             "cond quoted nil");
  TEST_EVAL("(cond ('() 1) (() 2))", NIL_STR,        "cond no true predicates");


  TEST_EVAL("(cond (0 'a) (t 'b))", "a",              "zero is true");
  TEST_EVAL("(cond ('(a) 'a) (t 'b)", "a",            "simple list is true");

  TEST_EVAL("(cond (t 'b) ())", "b",                 "Ignore malformed later predicate pairs");
  TEST_ERROR("(cond '())",                           "quote is not true");

  TEST_EVAL("(cond ((eq 'a 'b) 'first) ((atom 'a) 'second))",
            "second",                                 "cond of eq and atom");
  TEST_EVAL("(cond ((eq 'a 'b) 'first) ((atom '(a)) 'second) ((eq (car (cdr '(a b c))) 'b) (cdr '(x y z !))))",
            "(y z !)",                                 "really long cond");

  TEST_ERROR("(cond ())",                           "empty predicate pair");
  TEST_ERROR("(cond (() 'hello) ())",                "empty predicate pair second");
  TEST_ERROR("(cond t)",                            "predicate pair is not a list");
  TEST_ERROR("(cond (t))",                          "true predicate without value");
  TEST_ERROR("(cond (()) ())",                      "false predicate without value");
  TEST_ERROR("(cond (() a) ())",                    "false predicate then empty pair");
  TEST_ERROR("(cond (() a) (t))",                   "true predicate without value, second");
  TEST_ERROR("(cond () ())",                        "two empty predicate pairs");

  TEST_REPORT();
}

DEF_TEST(set) {
  TEST_INIT();

  SERIES(setx, "(set 'x 5)");
  TEST_EVALS(setx, "x", "5",                "simple set");

  SERIES(set_y,
         "(set 'y 5)",
         "(set 'y 10)");
  TEST_EVALS(set_y, "y", "10",              "simple over-write");

  SERIES(set_x_eval,
         "(set 'x (eq (car '(a b c)) 'a))");
  TEST_EVALS(set_x_eval,
             "(cond (x '5) ('() '6))", "5", "set as evaluation of another expression");

  SERIES(self_ref,
         "(set 'x '(1 2 3 4 5))",
         "(set 'x (cdr x))");
  TEST_EVALS(self_ref,
            "x", "(2 3 4 5)",              "self-referential over-write");

  SERIES(consy,
         "(set 'x '(1 2 3))",
         "(set 'x (cons x x))");
  TEST_EVALS(consy, "x", "((1 2 3) 1 2 3)", "over-write with self-referential cons");

  TEST_ERROR("(set)",                       "no arguments");
  TEST_ERROR("(set x)",                     "one argument");
  TEST_ERROR("(set x y z)",                 "too many arguments");
  TEST_ERROR("(set 1 4)",                   "can't set a number");
  TEST_ERROR("(set 5 6)",                   "can't set a number (again)");
  TEST_ERROR("(set t 4)",                   "can't set truth atom");
  TEST_ERROR("(set 't 4)",                  "can't set (quoted) truth atom");
  TEST_ERROR("(set () 4)",                  "can't set nil");
  TEST_ERROR("(set '() 4)",                 "can't set (quoted) nil");
  TEST_ERROR("(set '(a) 4)",                "can't set a list");
  TEST_ERROR("(set '(a b c) 6)",            "can't set a bigger list");
  TEST_ERROR("(set (lambda (x) x) 'e)",     "can't set a closure");

  TEST_REPORT();
}

DEF_TEST(math) {
  TEST_INIT();

  TEST_TRUE("(= 1 1)",                     "one equals one");
  TEST_FALSE("(= 1 0)",                    "one doesn't equal zero");

  TEST_EVAL("(+ 1 1)", "2",                 "one plus one equals two");
  TEST_EVAL("(+ 20 -25)", "-5",             "add negative number");
  TEST_EVAL("(- 13 7)", "6",                "simple subtraction");
  TEST_EVAL("(- 10 100)", "-90",            "subtraction, negative result");
  TEST_EVAL("(* 1337 0)", "0",              "multiply by zero");
  TEST_EVAL("(* 6 7)", "42",                "multiple two positive");

  TEST_EVAL("(/ 93 1)", "93",               "divide by one");
  TEST_EVAL("(/ 42 6)", "7",                "simple divide");
  TEST_EVAL("(/ 42 100)", "0",              "integer division");
  TEST_EVAL("-5", "-5",                     "negative number equals self");

  TEST_TRUE("(> 5 0)",                      "five is greater than zero");
  TEST_TRUE("(> (+ 4 1) 4)",                "greater than");
  TEST_FALSE("(> -5 0)",                    "negative not greater than zero");
  TEST_FALSE("(> 78 78)",                   "number is not greater than self");
  TEST_TRUE("(>= 4 4)",                     "Number is greater than or equal to self");
  TEST_TRUE("(>= 123 -123)",                "Number is greater than negative of itself");
  TEST_FALSE("(>= 0 5)",                    "Zero not greater than five");

  TEST_TRUE("(< 5 6)",                      "");
  TEST_TRUE("(< -1234 7)",                  "");
  TEST_FALSE("(< 9 9)",                     "");
  TEST_TRUE("(<= 4 4)",                     "");
  TEST_TRUE("(<= -123 123)",                "");
  TEST_FALSE("(<= 5 0)",                    "");

  SERIES(setx, "(set 'x 5)");
  TEST_EVALS(setx, "(+ x 5)", "10",         "");

  SERIES(set_xy,
         "(set 'x 7)",
         "(set 'y 13)");
  TEST_EVALS(set_xy, "(+ x y)", "20",       "");
  TEST_EVALS(set_xy, "(- x y)", "-6",       "");
  TEST_EVALS(set_xy, "(* x y)", "91",       "");
  TEST_EVALS(set_xy, "(/ y x)", "1",        "");
  TEST_EVALS(set_xy, "(% y x)", "6",        "");

  // weird ways to use these things
  // TEST_EVAL("(+ 1 2 3 4)", "10"); // just kidding, we will use the standard
  // TEST_EVAL("(* 1 2 3 4)", "24"); // library to implement this functionality

  // Data type errors
  TEST_ERROR("(+ 5 z)",                      "add with unknown variable");
  TEST_ERROR("(+ 5 ())",                     "can't add nil");
  TEST_ERROR("(+ 4 (lambda (x) (+ 2 x)))",   "can't add closure");
  TEST_ERROR("(/ t t)",                      "can't divide quote");
  TEST_ERROR("(/ 5 t)",                      "can't add truth");
  TEST_ERROR("(+ + +)",                      "can't add math primitives");
  TEST_ERROR("(/ / /)",                      "can't divide math primitives");

  TEST_ERROR("(>= e 4)",                     "can't compare unknown var");

  // Too few arguments
  TEST_ERROR("(+)",                          "no arguments");
  TEST_ERROR("(-)",                          "no arguments");
  TEST_ERROR("(*)",                          "no arguments");
  TEST_ERROR("(/)",                          "no arguments");
  TEST_ERROR("(=)",                          "no arguments");
  TEST_ERROR("(%)",                          "no arguments");
  TEST_ERROR("(% 3)",                        "one argument");
  TEST_ERROR("(/ 3)",                        "one argument");
  TEST_ERROR("(= 4)",                        "one argument");

  // Too many arguments
  TEST_ERROR("(- 3 4 5)",                    "too many arguments");
  TEST_ERROR("(/ 3 4 5)",                    "too many arguments");
  TEST_ERROR("(% 3 4 5)",                    "too many arguments");

  TEST_REPORT();
}

DEF_TEST(lambda) {
  TEST_INIT();

  TEST_EVAL("((lambda (x) (car x)) '(a b c))", "a",                      "simple car lambda");
  TEST_EVAL("((lambda (x) (cdr x)) '(a b c))",  "(b c)",                 "simple cdr lambda");
  TEST_EVAL("((lambda (x y) (cons x (cdr y))) 'a '(z b c))", "(a b c)",  "simple cons lambda");
  TEST_EVAL("((lambda (x) (cons 'z x)) '(a b c))", "(z a b c)",          "cons with z lambda");
  TEST_EVAL("((lambda (a b c) 5) 1 2 3)", "5",                           "ignore arguments");

  SERIES(zero,
         "(set 'y '(a b c))",
         "(set 'f  (lambda (x) (cons x y)))");
  TEST_EVALS(zero, "(f '(1 2 3))", "((1 2 3) a b c)",                   "set and apply lambda");

  SERIES(one,
         "(set 'y '(a b c))",
         "(set 'y '(4 5 6))",
         "(set 'f    (lambda (x) (cons x y)))");
  TEST_EVALS(one, "(f '(1 2 3))", "((1 2 3) 4 5 6)",            "apply lambda to reset argument");

  SERIES(two,
         "(set 'caar (lambda (x) (car (car x))))",
         "(set 'f    (lambda (x) (cons 'z x)))",
         "(set 'g    (lambda (x) (f (caar x))))");
  TEST_EVALS(two,"(g '(((a b) c) d) )", "(z a b)",              "nested closure application");

  SERIES(three,
         "(set 'make-adder (lambda (x) (lambda (y) (+ x y))))",
         "(set 'add-5 (make-adder 5)");
  TEST_EVALS(three, "(add-5 7)", "12",                          "first class lambda functions");

  SERIES(four,
         "(set 'make-prepender (lambda (x) (lambda (y) (cons x y))))",
         "(set 'prepend-z (make-prepender 'z))");
  TEST_EVALS(four, "(prepend-z '(a b c))", "(z a b c)",         "first class lambda functions 2");

  SERIES(five,
         "(set 'f (lambda () 4))",
         "(f)");
  TEST_EVALS(five, "(f)", "4",                                  "lambda no arguments");

  SERIES(six,
         "(set 'f (lambda () (+ 5 6)))",
         "(f)");
  TEST_EVALS(six, "(f)", "11",                                  "lambda no args");

  TEST_ERROR("(lambda)",                                        "no parameters or body");
  TEST_ERROR("(lambda 1)",                                      "no parameters");
  TEST_ERROR("(lambda one two three four)",                     "too many arguments");
  TEST_ERROR("(lambda t)",                                      "t can't be an argument");
  TEST_ERROR("(lambda (t) 99)",                                 "t really can't be an argument");
  TEST_ERROR("(lambda (5) 99)",                                 "numeric can't be argument");
  TEST_ERROR("(lambda (a t) 99)",                               "t can't be second argument");
  TEST_ERROR("(lambda (a b d c t) 99)",                         "t can't be n'th argument");
  TEST_ERROR("(lambda (a b 5 c t) 99)",                         "numeric can't be n'th argument");

  TEST_ERROR("(lambda (()) 99)",                                "nil can't be argument");
  TEST_ERROR("(lambda ('()) 99)",                               "nil can't be argument");
  TEST_ERROR("(lambda (a b () c e) 99)",                        "nil can't be n'th argument");

  TEST_REPORT();
}

DEF_TEST(closure) {
  TEST_INIT();

   SERIES(one,
          "(set 'f (lambda (x y) (+ x y)))",
          "(set 'add-5 (f 5)");
   TEST_EVALS(one, "(add-5 100)", "105",          "simple variable capture");


   SERIES(two,
           "(set 'x 8)",
           "(set 'double (lambda (x) (+ x x)))");
   TEST_EVALS(two, "(double 7)", "14",              "variable capture from environment");

   TEST_REPORT();
}

DEF_TEST(recursion) {
  TEST_INIT();

   SERIES(factorial,
          "(set 'factorial (lambda (x) "
            "(cond "
            "((= x 0) 1) "
            "(t (* x (factorial (- x 1)))))))");
   TEST_EVALS(factorial, "(factorial 5)", "120",        "factorial 5");
   TEST_EVALS(factorial, "(factorial 8)", "40320",      "factorial 8");
   TEST_EVALS(factorial, "(factorial 0)", "1",          "factorial 0");

   // ith element
   SERIES(ith,
          "(set 'ith (lambda (x i)"
            "(cond"
            "((= i 0) (car x))"
            "(t (ith (cdr x) (- i 1))))))");
   TEST_EVALS(ith, "(ith '(1 2 3 4 5) 2)", "3",       "recursive i'th function");

   SERIES(repeat,
          "(set 'repeat (lambda (item n)"
            "(cond"
            "((= n 1) item)"
            "(t (cons (car item) (repeat item (- n 1)))))))");
   TEST_EVALS(repeat, "(repeat '(3) 7)", "(3 3 3 3 3 3 3)",     "recursive repetition function");

  TEST_REPORT();
}

DEF_TEST(Y_combinator) {
  TEST_INIT();

  // A simple tail-recursive function using Y-Combinator
  SERIES(yc,
         "(set 'Y "
         " (lambda (r) "
         "  ((lambda (f) (f f)) "
         "   (lambda (f) (r (lambda (x) ((f f) x)))))))",

         "(set 'F "
         " (lambda (g) "
         "  (lambda (n) "
         "   (cond "
         "    ((= n 0) 42) "
         "    (t       (g (- n 1)))))))");

  TEST_EVALS(yc, "((Y F) 10)", "42",                     "Simple Y-Combinator");

  // The factorial functoin using the Y-Combinator
  SERIES(yc_factorial,
         "(set 'Y"
         " (lambda (r)"
         "  ((lambda (f) (f f))"
         "   (lambda (f) (r (lambda (x) ((f f) x)))))))",

         "(set 'F"
         " (lambda (g)"
         "  (lambda (n)"
         "   (cond "
         "    ((= n 0) 1)"
         "    (t       (* n (g (- n 1))))))))");

  TEST_EVALS(yc_factorial, "((Y F) 0)", "1",                   "Factorial 0 Y-Combinator");
  TEST_EVALS(yc_factorial, "((Y F) 1)", "1",                   "Factorial 1 Y-Combinator");
  TEST_EVALS(yc_factorial, "((Y F) 5)", "120",                 "Factorial 5 Y-Combinator");
  
  TEST_REPORT();
}

