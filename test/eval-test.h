
#ifndef LISP_EVAL_TEST_H
#define LISP_EVAL_TEST_H

#include <interpreter.h>
#include "test.h"


/**
 * Function: test_single_eval
 * --------------------------
 * Tests if a certain expression evaluated to the correct thing. Will print the
 * results to stdout
 * @param expr: The expression to evaluate
 * @param expected: The expected result of evaluating the expression
 * @return: True if the expression evaluated to the expected thing, false otherwise
 */
bool test_single_eval(const_expression expr, const_expression expected);

/**
 * Function: test_multi_eval
 * -------------------------
 * Tests if a single expression evaluates to the correct thing after first executing a series of
 * expressions beforehand
 * @param before: NULL terminated list of expressions to evaluate in the same environment beforehand
 * @param expr: The expression to evaluate after the list of expression, and verify the output of
 * @param expected: The expected output of evaluating expr
 * @return: True if expr evaluates to expected, and false otherwise
 */
bool test_multi_eval(const_expression before[],
                     const_expression expr, const_expression expected);

/**
 * Function: test_quote
 * --------------------
 * Tests if the quote language feature works correctly
 * @return: True if all the tests pass, false otherwise
 */
DEF_TEST(quote);

/**
 * Function: test_car_cdr
 * ----------------------
 * Tests for proper functionality of car and cdr, printing results to standard output
 * @return: True if all the tests passed, false otherwise
 */
DEF_TEST(car_cdr);

/**
 * Function: test_atom
 * -------------------
 * Tests the functionality of the atom primitive.
 * @return: True if all atom tests pass, false otherwise
 */
DEF_TEST(atom);

/**
 * Function: test_eq
 * -----------------
 * Tests the functionality of the eq primitive
 * @return: The number of tests that were failed
 */
DEF_TEST(eq);

/**
 * Function: test_cons
 * -------------------
 * Tests the functionality of the cons primitive
 * @return: The number of cons tests that failed
 */
DEF_TEST(cons);

/**
 * Function: test_cond
 * -------------------
 * Tests the functionality of the cond primitive
 * @return: The number of cond tests that
 */
DEF_TEST(cond);

/**
 * Function: test_set
 * ------------------
 * Tests whether something can be set correctly in the environment
 * @return: The number of tests that were failed
 */
DEF_TEST(set);

/**
 * Function: test_math
 * -------------------
 * Tests if arithmetic works correctly
 * @return: The number of tests that failed
 */
DEF_TEST(math);

/**
 * Function: test_lambda
 * ---------------------
 * Tests the functionality of the lambda function language feature
 * @return: The number of tests that failed
 */
DEF_TEST(lambda);

/**
 * Function: test_closure
 * ----------------------
 * Tests the functionality of creating closures with lambda functions
 * @return: The number of tests that failed
 */
DEF_TEST(closure);

/**
 * Function: test_recursion
 * ------------------------
 * Tests the functionality of using recursion with set and lambda functions
 * @return: The number of tests that failed
 */
DEF_TEST(recursion);

/**
 * Function: test_Y_combinator
 * ---------------------------
 * Tests the functionality of purely functional recursion in the lisp interpreter
 * @return: The number of tests that failed
 */
DEF_TEST(Y_combinator);

#endif //LISP_EVAL_TEST_H
