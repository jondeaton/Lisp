/**
 * @file cset.h
 * @brief Ordered set providing O(log n) time operations
 * @details This ordered set provides logarithmic time insertion, deletion, and
 * lookup as well as ranking operation. This implementation uses an AVL tree
 * to organize elements so as to provide these runtime guarantees.
 * All elements contained within the set are distinct. Insertion of an element
 * into a set which contains an equivalent element will result in no operation.
 * @author Jon Deaton
 * @date September 2, 2018
 */

#ifndef CLIB_CSET_H
#define CLIB_CSET_H

#include <ops.h>

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#endif

#define CSET_ERROR (-1)

typedef struct CSetImplementation CSet;

/**
 * Creates a new set for storing objects of size `data_size` that are compared
 * by function `cmp` and disposed of with function `cleanup`
 * @param data_size The size of each object to store in the set
 * @param cmp Comparison function. Must induce strict total order over elements
 * @param cleanup Function to use for disposing of elements
 * @return Pointer to a new set in dynamically allocated memory
 */
CSet *new_set(size_t data_size, CMapCmpFn cmp, CleanupFn cleanup);

/**
 * Insert a single element into the set.
 * @note Calling this function will not invalidate any pointers to elements previously
 * returned by `set_lookup`. If an element that is equivalent to `data` already exists
 * within the set this function will have no effect.
 * @param set The set to insert an element into
 * @param data Pointer to object of size `data_size` to copy and store
 * in the set data structure.
 */
void set_insert(CSet *set, const void *data);

/**
 * Lookup an element within the set.
 * @param set The set to lookup an element in
 * @param data Pointer to an object of size `data_size`. Will try to find an object
 * within the set equivalent to `data` in the strict order induced by `cmp`.
 * @return Pointer to an object within the set equivalent to `data` if such
 * an object exists in the set, otherwise NULL.
 */
void *set_lookup(const CSet *set, const void *data);

/**
 * Find the rank of an object.
 * @param set The set to find the rank of an object within
 * @param data Pointer to an object of size `data_size` to find the rank of.
 * @return The zero-indexed rank of the object. This is equal to the number of objects
 * contained in the set which are less than `data` in the strict order induced by `cmp`.
 */
int set_rank(CSet *set, const void *data);

/**
 * Get the size of the set.
 * @param set The set to find the size of
 * @return Number of elements contained within the set
 */
int set_size(const CSet *set);

/**
 * Remove an element from the set.
 * @param set The set to remove an element from
 * @param data Pointer to an object of size `data_size`. If there is an element
 * that is equivalent to `data`, it will be removed from the set, and the cleanup
 * function will be called on it. The size of the set will decrease by one
 */
void set_remove(CSet *set, const void *data);

/**
 * Remove all elements from the set.
 * @note Time complexity is O(n). The set is empty after this operation.
 * @param set The set to remove all elements from
 */
void set_clear(CSet *set);

/**
 * Dispose of the set and all of it's contents.
 * @note Any operation called on the set after this function is undefined behavior.
 * @param set The set to dispose of
 */
void set_dispose(CSet *set);


#ifdef __cplusplus
}
#endif

#endif //CLIB_CSET_H
