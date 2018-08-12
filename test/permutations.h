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
 * @enum Direction
 * @brief For indicating which direction an integer is
 * facing in the Steinhaus–Johnson–Trotter algorithm
 */
enum Direction { left = 0, right = 1 };

typedef struct permuter permuter;
typedef int (*CompareFn)(const void *addr1, const void *addr2);

/**
 * @brief Function for getting the n'th permutation of a string
 * @param string the string to get the permutation of (should be sorted)
 * @param n the n'th permutation
 * @param perm buffer to store the n'th permutation of the string
 */
void nth_permutation(const char *string, int n, char *perm);

/**
 * @brief creates a new permuter object
 * @param elems pointer to array of (sorted) contiguous elements to
 * iterate through permutations of
 * @param nelems the number of elements in the array
 * @param elem_size the size of each element
 * @param cmp comparison function
 * @return a new permuter object
 */
permuter *new_permuter(void *elems, int nelems, size_t elem_size, CompareFn cmp);

/**
 * @brief disposes of a permuter object
 * @param p permuter object to dispose of
 */
void permuter_dispose(permuter *p);

/**
 * @brief swaps elements to produce the next permutation
 * @details The core of the Steinhaus–Johnson–Trotter algorithm
 * @param p The permuter object
 * @return Pointer to the elements having been arranged in the new permutation
 */
void *next_permutation(permuter *p);

/**
 * @brief Populates a buffer with the n'th combination of an array of items
 * @param items Array of items to get a combination or
 * @param item_size The size of each item
 * @param n Which combination
 * @param combination Buffer to store the n'th combination
 */
void nth_combination(const void *items, size_t item_size, int n,
                     const void *end,
                     const void *combination);

/**
 * @brief Factorial function
 * @param n Integer to get the factorial of
 * @return n!
 */
int factorial(int n);

#endif // _PERMUTATIONS_H
