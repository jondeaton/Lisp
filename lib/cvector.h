/* File: cvector.h
 * ---------------
 * Defines the interface for the CVector type.
 * The CVector manages a linear, indexed collection of homogeneous elements.
 */

#ifndef _CVECTOR_H_INCLUDED
#define _CVECTOR_H_INCLUDED

#include <ops.h>

#include <stdbool.h>
#include <stddef.h>

#define for_vector(cv, el) for((el) = cvec_first(cv); (el) != NULL; (el) = cvec_next(cv, el))

/* Type: struct CVector
 * ----------------------------------
 * This definition completes the CVector type that was declared in
 * cvector.h. This struct contains data fields with information about
 * where the elements of the CVector are located, how many elements are currently
 * in the CVector, how many could possibly be in the CVector, element size
 * and a call-back function for cleaning up the elements.
 */
typedef struct CVector {
  void* elems;            // Pointer the contiguous elements of the CVector
  int nelems;             // Number of elements currently in the CVector
  int capacity;           // The maximum number of elements that could be stored
  size_t elemsz;          // The size of each element in bytes
  CleanupFn cleanup;      // Callback function for cleaning up an element
} CVector;

/**
 * Function: new_cvec
 * ------------------
 * Allocate a new CVector
 */
CVector *new_cvec(size_t elemsz, size_t capacity_hint, CleanupFn fn);

/**
 * Function: cvec_init
 * ---------------------
 * @param cvec The CVector to initialize
 * @param elemsz the size, in bytes, of the type of elements
 * that will be stored in the CVector. For example, to store elements of type
 * double, the client passes sizeof(double) for the elemsz. All elements
 * stored in a given CVector must be of the same type. An assert is
 * raised if elemsz is zero.
 * @param capacity_hint allows the client to tune the resizing behavior.
 * The CVector's internal storage will initially be allocated to hold the
 * number of elements hinted. This capacity_hint is not a binding limit.
 * If the initially allocated capacity is outgrown, the CVector enlarges its
 * capacity. If intending to store many elements, specifying a large capacity_hint
 * will result in an appropriately large initial allocation and fewer resizing
 * operations later. For a small vector, a small capacity_hint will result in
 * several smaller allocations and potentially less waste. If capacity_hint
 * is 0, an internal default value is used.
 * @param fn client callback function to cleanup an element. This
 * function will be called on an element being removed/replaced (via
 * cvec_remove/cvec_replace respectively) and on every element in the CVector
 * when it is destroyed (via cvec_dispose). The client can use this function
 * to do any deallocation/cleanup required for the element, such as freeing any
 * memory to which the element points (if the element itself is or contains a
 * pointer). The client can pass NULL for fn if elements don't require any cleanup.
 * @return Pointer to new CVector
 */
bool cvec_init(CVector *cvec, size_t elemsz, size_t capacity_hint, CleanupFn fn);

/**
 * Function: cvec_dispose
 * ----------------------
 * Disposes of the CVector. Calls the client's cleanup function on each element
 * and de-allocates memory used for the CVector's storage. Operates in linear-time.
 * Usage: cvec_dispose(v)
 */
void cvec_dispose(CVector *cv);

/**
 * Function: cvec_count
 * --------------------
 * Returns the number of elements currently stored in the CVector.
 * Operates in constant-time.
 * Usage: int count = cvec_count(v)
 */
int cvec_count(const CVector *cv);

/**
 * Function: cvec_nth
 * ---------------------------------------
 * Accesses the element at a given index in the CVector. Returns a
 * pointer to a memory location where the element value is stored.
 * Valid indexes are 0 to count-1. An assert is raised if index is out
 * of bounds. The return value is a pointer into the CVector's storage
 * so it must be used with care. In particular, a pointer returned by
 * cvec_nth can become invalid during any call that adds, removes, or
 * rearranges elements within the CVector. The CVector could have been
 * designed without this direct access, but it is useful and efficient to offer
 * it, despite its potential pitfalls. Operates in constant-time.
 *
 * Asserts: invalid index
 *
 * Usage: int num = *(int *)cvec_nth(v, 0)
 */
void *cvec_nth(const CVector *cv, int index);

/**
 * Function: cvec_insert
 * ---------------------
 * Inserts a new element into the CVector, placing it at the given index
 * and shifting up other elements to make room. An assert is raised if index
 * is less than 0 or greater than the count. addr is expected to be a valid
 * pointer to an element. For example, if this CVector has been created for
 * int elements, addr should be the memory location where the desired int
 * value is stored. The value at that location is copied into internal
 * CVector storage. The capacity is enlarged if necessary, an assert is raised
 * on allocation failure. Operates in linear-time.
 *
 * Asserts: invalid index, allocation failure
 * Assumes: address of valid elem
 *
 * Usage: cvec_insert(v, &elem, 0)
 */
void cvec_insert(CVector *cv, const void *source, int index);

/**
 * Function: cvec_append
 * ---------------------
 * Appends a new element to the end of the CVector. addr is expected to be a
 * valid pointer to an element. The element value is copied from the memory
 * location pointed to by addr. The capacity is enlarged if necessary, an
 * assert is raised on allocation failure. Operates in constant-time (amortized).
 *
 * Asserts: allocation failure
 * Assumes: address of valid elem
 *
 * Usage: cvec_append(v, &elem)
 */
void cvec_append(CVector *cv, const void *addr);

/**
 * Function: cvec_replace
 * ----------------------
 * Overwrites the element at the given index with a new element. Before
 * being overwritten, the client's cleanup function is called on the old
 * element. addr is expected to be a valid pointer to an element. The new
 * element value is copied from the memory location pointed to by addr and
 * replaces the old element at index. An assert is raised if index is
 * out of bounds. Operates in constant-time.
 *
 * Asserts: invalid index
 * Assumes: address of valid elem
 * Usage: cvec_replace(v, &elem, 0)
 */
void cvec_replace(CVector *cv, const void *addr, int index);

/**
 * Function: cvec_remove
 * ------------------------
 * Removes the element at the given index from the CVector and shifts
 * other elements down to close the gap. The client's cleanup function is
 * called on the element being removed. An assert is raised if index is
 * out of bounds. Operates in linear-time.
 *
 * Asserts: invalid index
 * Usage: cvec_remove(v, 0)
 */
void cvec_remove(CVector *cv, int index);

/**
 * Function: cvec_clear
 * -------------------
 * Disposes of all elements in the CVector by calling the cleanup function on all elements
 * @param cv: The CVector to clear of all elements
 */
void cvec_clear(CVector *cv);

/**
 * Function: cvec_search
 * ---------------------
 * Searches the CVector for an element matching a key element from a starting
 * index to the end of the vector. This function does not
 * re-arrange/modify elements within the CVector or modify the key.
 * Operates in linear-time or logarithmic-time (if sorted).
 *
 * An assert is raised if start is less than 0 or greater than the
 * CVector's count (although searching from count will never find anything,
 * allowing this case means client can search an empty CVector from 0 without
 * getting an assert).
 *
 * Asserts: valid start index
 * Assumes: address of valid key, cmp fn is valid
 *
 * @param cv: The CVector to search
 * @param keyaddr: A valid pointer to the element to search for
 * @param cmp: Callback function to use to compare elements
 * @param start: The place to search the serach from
 * @param sorted: The sorted parameter allows the client to specify that elements
 * are currently stored in sorted order, in which case cvec_search uses a
 * faster binary search. If sorted is false, linear search is used instead.
 * @return: If a match is found, the index of a matching element is returned;
 * else -1 is returned. If more than one match exists, any of the
 * matching indexes can be returned.
 */
int cvec_search(const CVector *cv, const void *keyaddr, CmpFn cmp, int start, bool sorted);

/**
 * Function: cvec_filter
 * ---------------------
 * Filters out elements of the CVector that do not match the predicate. The cleanup
 * function will be called for each element for which `predicate` does not return true.
 * Note: the vector will be in an invalid state for the duration of this function call. That means
 * don't use a `predicate` function that tries to access the CVector, it won't work.
 * Time complexity: O(n), Space complexity: O(1)
 * @param cv The CVector to filter elements out of
 */
void cvec_filter(CVector *cv, PredicateFn predicate);

/**
 * Function: cvec_sort
 * -------------------
 * Rearranges elements in the CVector into ascending order according to the
 * client's provided cmp callback. Operates in NlgN-time.
 *
 * Assumes: cmp fn is valid
 */
void cvec_sort(CVector *cv, CmpFn cmp);

/**
 * Function: cvec_first
 * --------------------
 * Provide iteration over the elements in conjunction with cvec_next.
 * @param cv: The CVector to get the first element of
 * @return: Pointer to the first element
 */
void *cvec_first(const CVector *cv);

/**
 * Function: cvec_next
 * -------------------
 * Provide iteration over the elements in conjunction with cvec_first.
 * @param cv: The CVector to get the next element of
 * @param prev: The previous element
 * @return: Pointer to the next element after the previous one
 */
void *cvec_next(const CVector *cv, const void *prev);

#endif // _CVECTOR_H_INCLUDED
