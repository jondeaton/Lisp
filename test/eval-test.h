//
// Created by Jonathan Deaton on 3/29/18.
//

#ifndef LISP_EVAL_TEST_H
#define LISP_EVAL_TEST_H

#include <interpreter.h>

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
bool test_multi_eval(const_expression before[], const_expression expr, const_expression expected);

int test_car_cdr();
int test_quote();
int test_atom();
int test_eq();
int test_cons();
int test_cond();
int test_set();
int test_math();
int test_lambda();
int test_closure();
int test_recursion();
int test_Y_combinator();

#endif //LISP_EVAL_TEST_H
