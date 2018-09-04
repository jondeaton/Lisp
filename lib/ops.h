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

// Generic cleanup function for use in containers
typedef void (*CleanupFn)(void *addr);

// Strict Total Order comparison function
typedef int (*CmpFn)(const void *keyA, const void *keyB, size_t keysize);

/**
 * String comparison functoin for pointers to C-strings (i.e. char**)
 * @details simply wraps `strcmp` by a single level of indirection
 * @param a Pointer to char* to compare with `b`
 * @param b Pointer to char* to compare with `a`
 * @param keysize unused parameter. the keysize of sizeof(char*)
 * @return the string comparison of `a` and `b`
 */
int cmp_cstr(const void *a, const void *b, size_t keysize UNUSED);

/**
 * Integer comparison function
 * @param intp1 Pointer to integer to compare against `intp2`
 * @param intp2 Pointer to integer to compare against `intp1`
 * @param keysize unused. The keysize is known to be sizeof(int)
 * @return 1 if *intp1 > *intp2, -1 *intp2 > *intp1, zero otherwise.
 */
int cmp_int(const void *intp1, const void *intp2, size_t keysize UNUSED);

/**
 * Pointer comparison
 * @param ptr1 pointer to pointer to compare against `ptr2`
 * @param ptr2 pointer to pointer to compare against `ptr1`
 * @param keysize unused. keysize is known to be sizeof(void*)
 * @return 1 if *ptr1 > *ptr2, -1 if *ptr2 > *ptr1, zero is pointers are equal.
 */
int cmp_ptr(const void *ptr1, const void *ptr2, size_t keysize UNUSED);

#ifdef __cplusplus
}
#endif


#endif //LISP_OPS_H
