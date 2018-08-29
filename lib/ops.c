
#include <ops.h>
#include <string.h>


int cmp_cstr(const void *a, const void *b, size_t keysize UNUSED) {
  return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_int(const void *intp1, const void *intp2) {
  return *(int *) intp1 - *(int *) intp2;
}
