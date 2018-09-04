
#include <ops.h>
#include <string.h>
#include <assert.h>

int cmp_cstr(const void *a, const void *b, size_t keysize UNUSED) {
  assert(a != NULL);
  assert(b != NULL);
  return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_int(const void *intp1, const void *intp2, size_t keysize UNUSED) {
  assert(intp1 != NULL);
  assert(intp2 != NULL);

  int one = *(int *) intp1;
  int two = *(int *) intp2;

  // note:  CANNOT just do "return one - two;" because of integer underflow!

  if (one > two) return 1;
  if (two > one) return -1;
  return 0;
}
