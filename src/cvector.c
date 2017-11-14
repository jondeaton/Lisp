/*
 * File: cvector.c
 * ---------------
 * This is the implementation of the CVector data structure. This implementation
 * uses a dynamically allocated struct to contain the CVector data, and a dynamically
 * allocated segment of memory to contain the elements of the vector contiguously.
 * This structure allows for elements of the vector to be accessed by index using
 * pointer arithmetic.
 */

#include "cvector.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <search.h>
#include <assert.h>

// A suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 16

/* Type: struct CVectorImplementation
 * ----------------------------------
 * This definition completes the CVector type that was declared in
 * cvector.h. This struct contains data fields with information about
 * where the elements of the CVector are located, how many elements are currently
 * in the CVector, how many could possibly be in the CVector, element size
 * and a call-back function for cleaning up the elements.
 */
struct CVectorImplementation {
  void* elems;            // Pointer the contiguous elements of the CVector
  int nelems;             // Number of elements currently in the CVector
  int capacity;           // The maximum number of elements that could be stored
  size_t elemsz;          // The size of each element in bytes
  CleanupElemFn cleanup;  // Callback function for cleaning up an element
};

// Function declarations
static void cvec_double_size(CVector* cv);
static inline size_t index_of(const CVector* cv, const void* elementp);

CVector* cvec_create(size_t elemsz, size_t capacity_hint, CleanupElemFn fn) {

  CVector* cvec = malloc(sizeof(CVector)); // Allocate space for the CVector struct on the heap
  cvec->nelems = 0; // Store starting

  // Use DEFAULT_CAPACITY if an invalid capacity_hint is given
  cvec->capacity = capacity_hint > 0 ? capacity_hint : DEFAULT_CAPACITY;
  cvec->elemsz = elemsz;

  // Store the callback function for laster cleanup of elements
  cvec->cleanup = fn;
  // Allocate space for the initial elements
  cvec->elems = malloc(cvec->capacity * elemsz);
  return cvec;
}

void cvec_dispose(CVector* cv) {
  // Only if elements need to be cleaned up do we need to loop thorugh them
  if (cv->cleanup != NULL) {
    // Loop through elements and call the cleanup callback for each
    for (void* elem = cvec_first(cv); elem != NULL; elem = cvec_next(cv, elem))
      cv->cleanup(elem);
  }
  free(cv->elems);
  free(cv);
}

int cvec_count(const CVector *cv) {
  return cv->nelems;
}

void* cvec_nth(const CVector* cv, int index) {
  assert(index >= 0 && index < cv->nelems);
  return (char*) cv->elems + index * cv->elemsz;
}

void cvec_insert(CVector* cv, const void* addr, int index) {
  // Assert valid index. 0 to nelems
  assert(index >= 0 && index <= cv->nelems);

  // If not enough room, double CVector size
  if (cv->capacity == cv->nelems) cvec_double_size(cv);

  // Number of bytes to shift over
  size_t num_bytes_shift = (cv->nelems - index) * cv->elemsz;

  // Use a buffer because memcpy with overlapping areas is undefined behavior
  char* buffer[num_bytes_shift];

  void* shift_source = (char*) cv->elems + index * cv->elemsz; // Can't use cvec_nth since we might be inserting at the end

  // todo: make this more memory efficient
  memcpy(buffer, shift_source, num_bytes_shift); // Copy elements after the insert sight into the buffer
  memcpy(shift_source, addr, cv->elemsz); // Copy the contents pointed to by address into the insertion site
  memcpy((char*) shift_source + cv->elemsz, buffer, num_bytes_shift); // Copy trailing elements back from the buffer
  cv->nelems++;
}

void cvec_append(CVector* cv, const void* addr) {
  if (cv->capacity == cv->nelems) cvec_double_size(cv);

  void* destination = (char*) cv->elems + cv->nelems * cv->elemsz;
  memcpy(destination, addr, cv->elemsz);
  cv->nelems++;
}

void cvec_replace(CVector* cv, const void* addr, int index) {
  void* destination = cvec_nth(cv, index); // Get nth element. Valid index will be asserted therein.
  if (cv->cleanup != NULL) (*cv).cleanup(destination); // Cleanup the old value if necessary
  memcpy(destination, addr, cv->elemsz); // Copy the new element into it's space
}

void cvec_remove(CVector* cv, int index) {
  void* to_be_removed = cvec_nth(cv, index); // Get the element. Valid index asserted therein

  // Cleanup element if necessary
  if (cv->cleanup != NULL) cv->cleanup(to_be_removed);

  // Get pointer to the elements after the one that was removed
  void* to_be_shifted = (char*) cv->elems + (index + 1) * cv->elemsz;

  // The number of bytes to shift down. If zero then nothing will happen.
  size_t num_bytes_shift = (cv->nelems - index - 1) * cv->elemsz;

  // Allocate buffer since memcpy doesn't work on overlapping regions
  // todo: make this more memory efficient
  char* buffer[num_bytes_shift];
  memcpy(buffer, to_be_shifted, num_bytes_shift);
  memcpy(to_be_removed, buffer, num_bytes_shift);

  cv->nelems--;
}

int cvec_search(const CVector* cv, const void* key, CompareFn cmp, int start, bool sorted) {
  assert(start >= 0 && start <= cv->nelems); // Assert start index is in range

  void* start_loc = (char*) cv->elems + start * cv->elemsz;
  size_t nitems = (size_t) cv->nelems - start;
  void* found_elem;

  if (sorted) // Binary search for sorted elements
    found_elem = bsearch(key, start_loc, nitems, cv->elemsz, cmp);
  else // Linear search for unsorted elements
    found_elem = lfind(key, start_loc, &nitems, cv->elemsz, cmp);

  if (found_elem == NULL) return -1; // Not found: -1
  else return (int) index_of(cv, found_elem);
}

void cvec_sort(CVector* cv, CompareFn cmp) {
  qsort(cv->elems, (size_t) cv->nelems, cv->elemsz, cmp);
}

void* cvec_first(const CVector* cv) {
  return cv->nelems > 0 ? cv->elems : NULL;
}

void* cvec_next(const CVector* cv, const void* prev) {
  if (prev < cv->elems) return NULL; // Check prev isn't before the first element

  // Check to see previous is the last element by pointer arithmetic
  bool is_last_element = index_of(cv, prev) == cv->nelems - 1;
  if (is_last_element) return NULL;

  return (char*) prev + cv->elemsz; // Advance pointer one element
}

/* Function: cvec_double_size
 * -------------------------
 * This function is for doubling the size of the CVector. The memory
 * used to store the elements will be reallocated (using realloc) to
 * request for a block of memory twice as big as the current capacity.
 * The capacity and elems fields in the CVector struct are updated to reflect
 * the change of size and (potential) change of memory location of elements.
 */
static void cvec_double_size(CVector* cv) {
  // Recall capacity is the number of elements that could be stored
  int new_capacity = 2 * cv->capacity;
  void* new_element_loc = realloc(cv->elems, cv->elemsz * new_capacity);

  assert(new_element_loc != NULL); // Assert memory allocation success

  cv->elems = new_element_loc;
  cv->capacity = new_capacity;
}

/**
 * Function: index_of
 * ------------------
 * Returns the index of an element pointed to inside of the CVector
 * @param cv: The CVector to get the index for
 * @param elementp: Pointer to an element in the CVector
 * @return: The index of the element pointed to
 */
static inline size_t index_of(const CVector* cv, const void* elementp) {
  return ((char*) elementp - (char*) cv->elems) / cv->elemsz;
}