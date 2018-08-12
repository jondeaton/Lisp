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
static inline void swap_adjacent(permuter *p, int i, enum Direction dir);
static enum Direction ith_direction(const permuter *p, int i);
static void set_ith_direction(permuter *p, int i, enum Direction direction);
static void flip_ith_direction(permuter *p, int i);
static int ith_false(const bool booleans[], size_t len, int i);

/**
 * @struct permuter
 * @brief encapsules the metadata required to perform the
 * Steinhaus–Johnson–Trotter algorithm so that the user of the
 * API doesn't need to understand the algorithm
 */
struct permuter {
  int n;                  // Number of elements to take the permutation of
  size_t elem_size;       // The size of each element
  void *elems;         // Pointer to the elements
  void *tmp;              // Temporary element pointer for swapping elements
  CompareFn cmp;          // Comparison function between elements
  char directions[];      // bit-array storing directions of each element
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

  reset_directions(p); // set all the directions to "left"
  return p;
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
  swap_adjacent(p, max_mobile_idx, ith_direction(p, max_mobile_idx));

  return p->elems;
}

void nth_combination(const void *items, size_t item_size, int n,
                     const void *end, const void *combination) {
  int i = 0;
  while (n != 0) {
    int first_offset = __builtin_ctz(n); // Find offset of first bit

    void *dst = (char *) combination + i * item_size;
    void *src = (char *) items + item_size * first_offset;

    memcpy(dst, src, item_size);

    n &= n - 1; // Turn that bit off
    i++;
  }
  memcpy((char *) combination + i * item_size, end, item_size); // add the end
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
  for (int i = 0; i < p->n / CHAR_BIT; ++i)
    p->directions[i] = 0;
}

static inline bool is_mobile(const permuter *p, int i) {
  assert(p != NULL);
  enum Direction dir = ith_direction(p, i);
  if (dir == left) {
    return i != 0 && compare(p, i, i - 1) > 0;
  } else {
    return i != p->n && compare(p, i, i + 1) > 0;
  }
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

static void flip_ith_direction(permuter *p, int i) {
  assert(p != NULL);
  int index = i / CHAR_BIT;
  char current = p->directions[index];
  char mask = (1 << (i % CHAR_BIT));
  p->directions[index] = current ^ mask;
}

static void set_ith_direction(permuter *p, int i, enum Direction direction) {
  assert(p != NULL);
  int index = i / CHAR_BIT;
  char current = p->directions[index];
  char mask = (1 << (i % CHAR_BIT));

  if (direction == left)
    p->directions[index] = current | ~mask;
  else
    p->directions[index] = current & mask;
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
