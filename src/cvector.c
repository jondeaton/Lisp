/*
 * File: cvector.c
 * ---------------
 * This is the implementation of the CVector data structure. This implementation
 * uses a dynamically allocated struct to contain the CVector data, and a dynamically
 * allocated segment of memory to contain the elements of the vector contiguously.
 * This structure allows for elements of the vector to be accessed by index using
 * pointer arithmetic.
 */

#include <cvector.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <search.h>
#include <assert.h>

// A suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 16

// Function declarations
static void cvec_double_size(CVector* cv);
static inline size_t index_of(const CVector* cv, const void* elementp);
static void* el_at_index(const CVector *cv, int index);


CVector *new_cvec(size_t elemsz, size_t capacity_hint, CleanupElemFn fn) {
  CVector* cvec = malloc(sizeof(CVector));
  if (cvec == NULL) return NULL;
  bool success = cvec_init(cvec, elemsz, capacity_hint, fn);
  if (!success) {
    free(cvec);
    return NULL;
  }
  return cvec;
}

bool cvec_init(CVector *cvec, size_t elemsz, size_t capacity_hint, CleanupElemFn fn) {
  assert(cvec != NULL);
  cvec->nelems = 0; // Store starting

  // Use DEFAULT_CAPACITY if an invalid capacity_hint is given
  cvec->capacity = (int) (capacity_hint > 0 ? capacity_hint : DEFAULT_CAPACITY);
  cvec->elemsz = elemsz;
  cvec->cleanup = fn; // Store the callback function for laster cleanup of elements
  cvec->elems = malloc(cvec->capacity * elemsz); // Allocate space for the initial elements
  if (cvec->elems == NULL) return false;
  return true;
}

void cvec_dispose(CVector* cv) {
  assert(cv != NULL);
  cvec_clear(cv);
}

int cvec_count(const CVector *cv) {
  assert(cv != NULL);
  return cv->nelems;
}

void* cvec_nth(const CVector* cv, int index) {
  assert(index >= 0 && index < cv->nelems);
  return el_at_index(cv, index);
}

void cvec_insert(CVector* cv, const void* source, int index) {
  assert(cv != NULL);
  assert(index >= 0 && index <= cv->nelems); // Assert valid index
  if (cv->capacity == cv->nelems) cvec_double_size(cv); // Resize vector if at capacity

  // Loop through the elements after the target backwards, copying them forward
  for (int i = cv->nelems; i > index; i--) {
    void* next_target = el_at_index(cv, i);
    void* next_source = el_at_index(cv, i - 1);
    memcpy(next_source, next_target, cv->elemsz);
  }
  void* target = el_at_index(cv, index);
  memcpy(target, source, cv->elemsz); // insert the
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
  void* el = cvec_nth(cv, index); // Get the element. Valid index asserted therein
  if (cv->cleanup != NULL) cv->cleanup(el); // Cleanup if necessary
  for(void* next = cvec_next(cv, el); next != NULL; next = cvec_next(cv, next))
    memcpy((char*) next - cv->elemsz, next, cv->elemsz);
  cv->nelems--;
}

void cvec_clear(CVector* cv) {
  if (cv->cleanup == NULL) {
    cv->nelems = 0;
    return;
  }
  for (void* el = cvec_first(cv); el != NULL; el = cvec_next(cv, el))
    cv->cleanup(el);
  cv->nelems = 0;
}

int cvec_search(const CVector* cv, const void* key, CompareFn cmp, int start, bool sorted) {
  assert(start >= 0 && start <= cv->nelems); // Assert start index is in range

  void* start_loc = (char*) cv->elems + start * cv->elemsz;
  size_t nitems = (size_t) cv->nelems - start;
  void* found_elem;

  // Search with binary or linear search if sorted or not, respectively
  if (sorted) found_elem = bsearch(key, start_loc, nitems, cv->elemsz, cmp);
  else found_elem = lfind(key, start_loc, &nitems, cv->elemsz, cmp);

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
  bool is_last_element = (int) index_of(cv, prev) == cv->nelems - 1;
  if (is_last_element) return NULL;

  return (char*) prev + cv->elemsz; // Advance pointer one element
}

static void cvec_double_size(CVector* cv) {
  // Recall capacity is the number of elements that could be stored
  int new_capacity = 2 * cv->capacity;
  void* new_element_loc = realloc(cv->elems, cv->elemsz * new_capacity);

  assert(new_element_loc != NULL); // Assert memory allocation success

  cv->elems = new_element_loc;
  cv->capacity = new_capacity;
}

static inline size_t index_of(const CVector* cv, const void* elementp) {
  return ((char*) elementp - (char*) cv->elems) / cv->elemsz;
}

static void* el_at_index(const CVector *cv, int index) {
  return (char*) cv->elems + index * cv->elemsz;
}
