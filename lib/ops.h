#ifndef LISP_OPS_H
#define LISP_OPS_H

#include <string.h>

#define unused __attribute__ ((unused))


#ifdef __cplusplus
extern "C" {
#endif

  // todo: rename these
typedef void (*CleanupFn)(void *addr);

typedef int (*CMapCmpFn)(const void *keyA, const void *keyB, size_t keysize);

typedef struct CMapImplementation CMap;

int cmp_cstr(const void *a, const void *b, size_t keysize unused) {
  return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_int(const void *intp1, const void *intp2) {
  return *(int *) intp1 - *(int *) intp2;
}

#ifdef __cplusplus
}
#endif


#endif //LISP_OPS_H
