#include <permutations.h>
#include <assert.h>
#include <limits.h>

// static forward declarations
static inline int div_round_up(int numer, int denom);
static inline int find_largest_mobile(const permuter *p);
static void reset_directions(permuter *p);
static int compare(const permuter *p, int i, int j);
static inline bool is_mobile(const permuter *p, int i);
static inline void *permuter_ith(const permuter *p, int i);
static inline void swap(permuter *p, int i, int j);
static inline void swap_facing(permuter *p, int i);
static inline void swap_adjacent(permuter *p, int i, enum Direction dir);
static enum Direction ith_direction(const permuter *p, int i);
static inline void set_ith_direction(permuter *p, int i, enum Direction direction);
static inline void flip_ith_direction(permuter *p, int i);
static int ith_false(const bool booleans[], size_t len, int i);

/**
 * @struct permuter
 * @brief encapsules the metadata required to perform the
 * Steinhaus–Johnson–Trotter algorithm so that the user of the
 * API doesn't need to understand the algorithm
 */
struct permuter {
  int n;                  // number of elements to take the permutation of
  size_t elem_size;       // the size of each element
  void *elems;            // pointer to the elements
  void *tmp;              // temporary element pointer for swapping elements
  CompareFn cmp;          // comparison function between elements
  int index;              // number of the current permutation
  char directions[];      // bit-array storing directions of each element of size n
};

permuter *new_permuter(void *elems, int nelems, size_t elem_size, CompareFn cmp) {
  assert(elems != NULL);
  permuter *p = malloc(sizeof(permuter) + div_round_up(nelems, CHAR_BIT));
  if (p == NULL) return NULL;

  // Allocate space for temporary element used to swapping
  p->tmp = malloc(elem_size);
  if (p->tmp == NULL) {
    free(p);
    return NULL;
  }

  p->n = nelems;
  p->elem_size = elem_size;
  p->elems = elems;
  p->cmp = cmp;

  reset_permuter(p); // set all the directions to "left"
  return p;
}

void *get_permutation(const permuter *p) {
  assert(p != NULL);
  return p->elems;
}

int permutation_index(const permuter *p) {
  assert(p != NULL);
  return p->index;
}

permuter *new_cstring_permuter(const char *string) {
  assert(string != 0);
  char *str = strdup(string);
  qsort(str, strlen(str), sizeof(char), cmp_char);
  return new_permuter(str, strlen(str), sizeof(char), cmp_char);
}

void cstring_permuter_dispose(permuter *p) {
  assert(p != NULL);
  free(p->elems);
  permuter_dispose(p);
}

int cmp_char(const void *pchar1, const void *pchar2) {
  assert(pchar1 != NULL);
  assert(pchar2 != NULL);
  char char1 = *(const char *) pchar1;
  char char2 = *(const char *) pchar2;
  return char1 - char2;
}

void permuter_dispose(permuter *p) {
  assert(p != NULL);
  free(p->tmp);
  free(p);
}

void *next_permutation(permuter *p) {
  assert(p != NULL);

  int max_mobile_idx = find_largest_mobile(p);
  if (max_mobile_idx < 0)
    return NULL; // no more permutations

  // flip the direction of all elements larger than the swap element
  for (int i = 0; i < p->n; ++i)
    if (compare(p, i, max_mobile_idx) > 0)
      flip_ith_direction(p, i);

  // swap the largest mobile element with the element adjacent to
  // it in the direction that it is facing
  swap_facing(p, max_mobile_idx);

  p->index++;
  return p->elems;
}

void reset_permuter(permuter *p) {
  assert(p != NULL);
  qsort(p->elems, p->n, p->elem_size, p->cmp);
  reset_directions(p);
  p->index = 0;
}

int permuter_size(permuter *p) {
  assert(p != NULL);
  return p->n;
}

void nth_combination(const void *elements, size_t elem_size, int n,
                     const void *end, const void *combination) {
  int i = 0;
  while (n != 0) {
    int first_offset = __builtin_ctz(n); // Find offset of first bit

    void *dst = (char *) combination + i * elem_size;
    void *src = (char *) elements + elem_size * first_offset;

    memcpy(dst, src, elem_size);

    n &= n - 1; // Turn that bit off
    i++;
  }
  memcpy((char *) combination + i * elem_size, end, elem_size); // add the end
}

int factorial(int n) {
  int f = 1;
  for (int i = 2; i <= n; ++i)
    f *= i;
  return f;
}

static inline int find_largest_mobile(const permuter *p) {
  assert(p != NULL);
  int max_mobile_idx = -1;
  for (int i = 0; i < p->n; ++i) {
    if (!is_mobile(p, i)) continue;
    if (max_mobile_idx == -1 || compare(p, i, max_mobile_idx) > 0)  {
      max_mobile_idx = i;
    }
  }
  return max_mobile_idx;
}

static void reset_directions(permuter *p) {
  assert(p != NULL);
  for (int i = 0; i < div_round_up(p->n, CHAR_BIT); ++i)
    p->directions[i] = 0;
}

static inline bool is_mobile(const permuter *p, int i) {
  assert(p != NULL);
  enum Direction dir = ith_direction(p, i);
  if (dir == left) {
    return i > 0 && compare(p, i, i - 1) > 0;
  } else {
    return i < (p->n - 1) && compare(p, i, i + 1) > 0;
  }
}

static inline void swap_facing(permuter *p, int i) {
  assert(p != NULL);
  assert(i >= 0 && i < p->n);
  swap_adjacent(p, i, ith_direction(p, i));
}

static inline void swap_adjacent(permuter *p, int i, enum Direction dir) {
  assert(p != NULL);
  assert(i >= 0 && i < p->n);

  int adjacent = (dir == left) ? i - 1 : i + 1;
  swap(p, i, adjacent);
}

static inline void swap(permuter *p, int i, int j) {
  assert(p != NULL);
  assert(i >= 0 && i < p->n);
  assert(j >= 0 && j < p->n);

  enum Direction dir_i = ith_direction(p, i);
  set_ith_direction(p, i, ith_direction(p, j));
  set_ith_direction(p, j, dir_i);

  memcpy(p->tmp, permuter_ith(p, i), p->elem_size);
  memcpy(permuter_ith(p, i), permuter_ith(p, j), p->elem_size);
  memcpy(permuter_ith(p, j), p->tmp, p->elem_size);
}

static int compare(const permuter *p, int i, int j) {
  assert(p != NULL);
  assert(i >= 0 && i < p->n);
  assert(j >= 0 && j < p->n);
  return p->cmp(permuter_ith(p, i), permuter_ith(p, j));
}

static inline void *permuter_ith(const permuter *p, int i) {
  assert(p != NULL);
  assert(i >= 0);
  return (char *) p->elems + i * p->elem_size;
}

static enum Direction ith_direction(const permuter *p, int i) {
  assert(p != NULL);
  bool bit = p->directions[i / CHAR_BIT] & (1 << (i % CHAR_BIT));
  return bit ? right : left;
}

static inline void flip_ith_direction(permuter *p, int i) {
  assert(p != NULL);
  enum Direction dir = ith_direction(p, i);
  set_ith_direction(p, i, dir == left ? right : left);
}

static inline void set_ith_direction(permuter *p, int i, enum Direction direction) {
  assert(p != NULL);
  int index = i / CHAR_BIT;
  char current = p->directions[index];
  char mask = (1 << (i % CHAR_BIT));

  if (direction == left)
    p->directions[index] = current & ~mask;
  else
    p->directions[index] = current | mask;
}

static inline int div_round_up(int numer, int denom) {
  if (numer == 0) return 0;
  return 1 + (numer - 1) / denom;
}

void nth_permutation(const char *string, int n, char *perm) {
  size_t len = strlen(string);
  bool *used = calloc(sizeof(bool) * len, 1);

  for (int i = (int) len - 1; i >= 0; --i) {
    int digit = n / factorial(i);

    // Get the "digit"'th unused character
    int index = ith_false(used, len, digit);
    used[index] = true;
    perm[i] = string[index];

    // reduce permutation number by one digit
    n = n % factorial(i);
  }
  perm[len] = '\0'; // null terminate permutation
  free(used);
}

static int ith_false(const bool booleans[], size_t len, int i) {
  for (int j = 0; j < (int) len; ++j) {
    if (booleans[j]) continue;
    if (i == 0) return j;
    i--;
  }
  return -1;
}

#if defined(DEBUG)

#include "stdio.h"

bool permutation_correctness_test() {

  permuter *p = new_cstring_permuter("123");
  if (p == NULL) return false;

  const char* correct_permutation[] = {"123", "132", "312",
                                       "321", "231", "213"};

  for (int repeat = 0; repeat < 3; repeat++) {
    for (const char *str = get_permutation(p); str != NULL; str = next_permutation(p)) {
      int i = permutation_index(p);
      if (strcmp(str, correct_permutation[i]) != 0) {
        cstring_permuter_dispose(p);
        return false;
      }
    }

    if (next_permutation(p) != NULL) {
      cstring_permuter_dispose(p);
      return false;
    }
    reset_permuter(p);
  }
  cstring_permuter_dispose(p);

  // Make sure that you generate 10! permutations of this string

  const char *strs[] = {"0", "01", "012", "01234", "012345", "0123456", "01234567" , NULL};

  for (int i = 0; strs[i] != NULL; i++) {
    p = new_cstring_permuter(strs[i]);
    if (p == NULL) return false;

    // rinse and repeat three times
    for (int repeat = 0; repeat < 3; repeat++) {
      int n_perms = 0;
      for (const char *str = get_permutation(p); str != NULL; str = next_permutation(p)) {
        n_perms += 1;
        if ((int) strlen(str) != permuter_size(p)) {
          cstring_permuter_dispose(p);
          return false;
        }
      }

      if (n_perms != factorial(permuter_size(p))){
        cstring_permuter_dispose(p);
        return false;
      }
      reset_permuter(p);
    }

    cstring_permuter_dispose(p);
  }

  return true;
}

bool combination_correctness_test() {
  // todo
  return true;
}

#endif // DEBUG
