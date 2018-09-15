
#include <ops.h>
#include <string.h>
#include <assert.h>

int cmp_cstr(const void *a, const void *b) {
  assert(a != NULL);
  assert(b != NULL);
  return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_int(const void *intp1, const void *intp2) {
  assert(intp1 != NULL);
  assert(intp2 != NULL);

  int one = *(int *) intp1;
  int two = *(int *) intp2;

  // note:  CANNOT just do "return one - two;" because of integer underflow!

  if (one > two) return 1;
  if (two > one) return -1;
  return 0;
}

int cmp_ptr(const void *ptr1, const void *ptr2) {
  assert(ptr1 != NULL);
  assert(ptr2 != NULL);

  const unsigned long p1 = *(const unsigned long*) ptr1;
  const unsigned long p2 = *(const unsigned long*) ptr2;
  if (p1 > p2) return 1;
  if (p2 > p1) return -1;
  return 0;
}
