#ifndef LISP_CSET_H
#define LISP_CSET_H

#include <ops.h>

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#endif

#define CSET_ERROR (-1)

typedef struct CSetImplementation CSet;

CSet *new_set(size_t data_size, CMapCmpFn cmp, CleanupFn cleanup);

void set_insert(CSet *set, const void *data);
void *set_lookup(const CSet *set, const void *data);
int set_rank(CSet *set, const void *data);
int set_size(const CSet *set);
void set_remove(CSet *set, const void *data);
void set_clear(CSet *set);
void set_dispose(CSet *set);


#ifdef __cplusplus
}
#endif

#endif //LISP_CSET_H
