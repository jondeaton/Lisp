#ifndef LISP_OPS_H
#define LISP_OPS_H

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifdef __cplusplus
#include <cstring>
extern "C" {
#else
#include <string.h>
#endif


// todo: rename these
typedef void (*CleanupFn)(void *addr);

typedef int (*CMapCmpFn)(const void *keyA, const void *keyB, size_t keysize);

typedef struct CMapImplementation CMap;

int cmp_cstr(const void *a, const void *b, size_t keysize UNUSED);

int cmp_int(const void *intp1, const void *intp2, size_t keysize UNUSED);

#ifdef __cplusplus
}
#endif


#endif //LISP_OPS_H
