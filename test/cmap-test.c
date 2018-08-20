#include <cmap-test.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <alphabet.h>

static bool test_creation();
static bool test_insertion(unsigned int capacity);
static bool test_large_insertion(unsigned int capacity, const char *string);
static bool test_deletion(unsigned int capacity);

bool cmap_correctness_test() {
  printf("Testing creation of Hash Table... ");
  bool success = test_creation();
  printf("%s\n", success ? "success" : "failure");

  printf("Testing insertion into Hash Table... ");
  for (unsigned int capacity = 30; capacity < 200; capacity += 10) {
    success = test_insertion(capacity);
    if (!success) break;
  }
  printf("%s\n", success ? "success" : "failure");

  printf("Testing large insertion into Hash Table... ");
  success = test_large_insertion(50000, "12345678");
  printf("%s\n", success ? "success" : "failure");

  printf("Testing deletion from Hash Table... ");
  for (unsigned int capacity = 1440; capacity < 1000000; capacity = (int) capacity * 1.1) {
    success = test_deletion(capacity);
    if (!success) break;
  }
  printf("%s\n", success ? "success" : "failure");
  return 0;
}

// Testing to make sure that you can use all kinds
// of key and value sizes
static bool test_creation() {
  for (size_t keysize = 1; keysize <= 32; keysize++) {
    for (size_t valuesize = 1; valuesize <= 32; valuesize++) {

      CMap *cm = cmap_create(keysize, valuesize,
                              NULL, NULL, NULL, NULL, (unsigned int) (keysize * valuesize));

      if (cm == NULL) return false;
      if (cmap_count(cm) != 0) return false;

      cmap_dispose(cm);
    }
  }
  return true;
}

static bool test_insertion(unsigned int capacity) {
  CMap *cm = cmap_create(sizeof(char *), sizeof(int),
                         string_hash, cmp_cstr,
                         NULL, NULL, capacity);
  if (cm == NULL)
    return false;

  if (cmap_count(cm) != 0)
    return false;

  for (int i = 0; i < NUM_LETTERS; ++i) {
    const char *const letter = alphabet[i];
    cmap_insert(cm, &letter, &i);
  }

  if (cmap_count(cm) != NUM_LETTERS)
    return false;

  // Make sure they're all in there
  for (int i = 0; i < NUM_LETTERS; ++i) {
    const char *const letter = alphabet[i];
    const void *l = cmap_lookup(cm, &letter);

    if (l == NULL)
      return false;

    if (*(int*) l != i)
      return false;
  }

  return true;
}

static bool test_large_insertion(unsigned int capacity, const char *string) {

  CMap *cm = cmap_create(sizeof(char *), sizeof(int), string_hash, cmp_cstr,
                         free, NULL, capacity);
  if (cm == NULL) return false;

  // insert every permutation of the string into the map
  permuter *p = new_cstring_permuter(string);
  const char *str;
  for_permutations(p, str) {
    int i = permutation_index(p);
    char *s = strdup(str);
    cmap_insert(cm, &s, &i);
  }

  reset_permuter(p);

  // Check to make sure that they're all there
  for_permutations(p, str) {
    int *valuep = cmap_lookup(cm, &str);
    if (valuep == NULL || *valuep != permutation_index(p))
      return false;
  }

  cstring_permuter_dispose(p);
  return true;
}

static bool test_deletion(unsigned int capacity) {
  CMap *cm = cmap_create(sizeof(char *), sizeof(int),
                         string_hash, cmp_cstr,
                         NULL, NULL, capacity);

  if (cm == NULL) return false;
  
  if (cmap_count(cm) != 0)
    return false;

  // insert all number permutations into the cmap
  permuter *p1 = new_cstring_permuter("012345");
  for (const char *str = get_permutation(p1); str != NULL; str = next_permutation(p1)) {
    int i = permutation_index(p1);
    char *s = strdup(str);
    cmap_insert(cm, &s, &i);
  }

  // insert all letter permutations into the cmap
  permuter *p2 = new_cstring_permuter("abcdef");
  for (const char *str = get_permutation(p2); str != NULL; str = next_permutation(p2)) {
    int i = permutation_index(p2);
    char *s = strdup(str);
    cmap_insert(cm, &s, &i);
  }

  reset_permuter(p1);
  reset_permuter(p2);

  // remove all the number permutations
   for (const char *str = get_permutation(p1); str != NULL; str = next_permutation(p1))
     cmap_remove(cm, &str);
   
   // check to make sure that all the letter permutations are there and correct
   for (const char *str = get_permutation(p2); str != NULL; str = next_permutation(p2)) {
    int *valuep = cmap_lookup(cm, &str);
    if (valuep == NULL)
      return false;
    
    if (*valuep != permutation_index(p2))
      return false;
  }

  cstring_permuter_dispose(p1);
  cstring_permuter_dispose(p2);

  cmap_dispose(cm);
  return true;
}
