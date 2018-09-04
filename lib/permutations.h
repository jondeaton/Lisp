/**
 * @file permutations.h
 * @brief For iterating through all permutations or combinations
 * of a collection of elements. Uses the Steinhaus–Johnson–Trotter algorithm
 * to iterate through permutations efficiently.
 *
 * @example simple permutation iteration with array of integers
 *
 * int arr[] = { 1, 2, 3, 4 };
 *
 * permuter *p = new_permuter(arr, 4, sizeof(int), cmp_int);
 * for (void *permutation = get_permutation(p);
 *      permutation != NULL;
 *      permutation = next_permutation(p)) {
 *
 *      // do something with permutation
 *  }
 *  permuter_dispose(p);
 *
 * alternatively, there is a nice macro that will accomplish the same thing with
 *
 * permuter *p = new_permuter(arr, 4, sizeof(int), cmp_int);
 * void *permutation;
 * for_permutations(p, permutation) {
 *    // use permutation
 * }
 *
 *  @example simple permutation of C-strings
 *
 * permuter *p = new_cstring_permuter("ABCDEFG");
 * for (const char *str = get_permutation(p);
 *      str != NULL;
 *      str = next_permutation(p)) {
 *
 *      // do something with str
 *  }
 * cstring_permuter_dispose(p);
 *
 */

#ifndef _PERMUTATIONS_H
#define _PERMUTATIONS_H

#ifdef __cplusplus

#include <cstdlib>
#include <cstdbool>
#include <cstring>
extern "C" {

#else

#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#endif

#define for_permutations(permuter, permutation) for((permutation) = get_permutation(permuter); \
                                                    (permutation) != NULL; \
                                                    (permutation) = next_permutation(permuter))

/**
 * @enum Direction
 * @brief For indicating which direction an integer is
 * "facing" in the Steinhaus–Johnson–Trotter algorithm
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
 * @brief Creates a permuter for a mutable C-string
 * @param string pointer to a C-string (will be copied)
 * @return permuter object for that C string
 */
permuter *new_cstring_permuter(const char *string);

/**
 * @brief disposes of a C-string permuter
 * @param p pointer to the c string permuter returned by
 * the new_cstring_permuter function
 */
void cstring_permuter_dispose(permuter *p);

/**
 * @brief swaps elements to produce the next permutation
 * @details Steinhaus–Johnson–Trotter algorithm. This implementation
 * has time complexity O(n) where n is the number of elements in the permutation.
 * This method will rearrange the elements in-place. Thus, to access the permutation
 * you can either use the return value from this function, or the pointer to the
 * elements that was originally passed to "new_permuter".
 * @param p The permuter object that was returned from new_permuter
 * @return pointer to the elements having been arranged in the new permutation
 */
void *next_permutation(permuter *p);

/**
 * @brief get the current permutation from the permuter.
 * @note no elements will be swapped. This function makes no changes
 * to the state of the permuter or permutation.
 * @param p The permuter object that was returned from new_permuter
 * @return The current permutation
 */
void *get_permutation(const permuter *p);

/**
 * @brief gets the index of the current permutation
 * @note will be between 0 and k! - 1 (inclusive)
 * @param p the permuter to get the number of
 * @return the index of the current permutation
 */
int permutation_index(const permuter *p);

/**
 * @brief resets the permuter so that it can be iterated through again
 * @details sorts the elements and resets all the directions back to "left"
 * @param p the permuter to reset
 */
void reset_permuter(permuter *p);

/**
 * @brief returns the number of elements that are in each permutation
 * @param p the permuter to get the size of
 * @return the number of elements that are in each permutation
 */
int permuter_size(permuter *p);

/**
 * @brief Comparison function for characters
 * @note useful for making permutations of C-strings: pass this in
 * as the comparison function to do C-string permutations.
 * @param pchar1 pointer to first character
 * @param pchar2 pointer to second character
 * @return difference between the two characters pointed to
 */
int cmp_char(const void *pchar1, const void *pchar2);

/**
 * @brief Populates a buffer with the n'th combination of an array of items
 * @details this method can be used to efficiently iterate through all combinations of a
 * series of elements by calling this function in a loop for values of n from
 * 0 to 2^k - 1 where k = the number of items. You don't need to pass the length
 * of the array to items to long as your value of n is less than 2^k. Values of
 * n greater of this will attempt to read values from beyond the end of the items
 * array and will result in undefined behavior.
 * The time complexity of this algorithm is equal to O(b) where b = the number of
 * bits that are in n. Said differently - time complexity is linear in the number
 * of elements that will be copied into the combinations buffer. Ensure that the
 * buffer is large enough to hold that many elements
 * @param elements array of k elements to make a combination of
 * @param elem_size the size of each element in the array
 * @param n number between 0 and 2^k - 1 (inclusive) denoting which combination
 * of the elements to copy into the combinations buffer
 * @param end pointer to an element that will be copied at the end of the combinations
 * array to indicate the end of the combination. For instance if you are taking
 * combinations of character arrays represented as C-strings then pass a pointer
 * to a null-terminator so that the resulting combination is a valid C-string.
 * @param combination buffer to store the n'th combination. Must be large enough to hold
 * as many elements as there are in the elements array plus one extra for the "end" element.
 */
void nth_combination(const void *elements, size_t elem_size, int n,
                     const void *end,
                     const void *combination);

/**
 * @brief Factorial function
 * @param n Integer to get the factorial of
 * @return n!
 */
int factorial(int n);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PERMUTATIONS_H
