/**
 * @file permutations.h
 * @brief For iterating through all permutations of a collection
 */

#ifndef _PERMUTATIONS_H
#define _PERMUTATIONS_H

#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

/**
 * @brief Function for getting the n'th permutation of a string
 * @param string the string to get the permutation of (should be sorted)
 * @param n the n'th permutation
 * @param perm buffer to store the n'th permutation of the string
 */
void nth_permutation(const char *string, int n, char *perm);

/**
 * @brief Factorial function
 * @param n Integer to get the factorial of
 * @return n!
 */
int factorial(int n);

#endif // _PERMUTATIONS_H
