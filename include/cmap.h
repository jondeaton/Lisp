/**
 * @file hashtable.h
 * @breif Defines the interface for the CMap type.
 */

#ifndef _CMAP_H
#define _CMAP_H

#include <stddef.h>
#include <stdbool.h>

typedef void (*CleanupFn)(void *addr);
typedef unsigned int (*CMapHashFn)(const void *key, size_t keysize);
typedef int (*CMapCmpFn)(const void *keyA, const void *keyB, size_t keysize);
typedef struct CMapImplementation CMap;

int string_cmp(const void *a, const void *b, size_t keysize);

/**
 * Create a HashTable in a dynamically allocated region of memory.
 * @param key_size size of all keys stored in HashTable
 * @param value_size size of all values stored in the HashTable
 * @param hash Hash function used to hash keys, may be NULL
 * @param cmp Comparison function between keys, may be NULL
 * @param cleanupKey Cleanup function for keys, may be NULL
 * @param cleanupValue Cleanup function for values for, may be NULL
 * @param capacity number of values that maybe stored in table
 * @return Pointer to a hash table in dynamically allocated memory
 */
CMap *cmap_create(size_t key_size, size_t value_size,
                  CMapHashFn hash, CMapCmpFn cmp,
                  CleanupFn cleanupKey, CleanupFn cleanupValue,
                  unsigned int capacity_hint);

/**
 * Dispose of a Hash Table created from cmap_create
 * @param cm Pointer to hash table
 */
void cmap_dispose(CMap *cm);

/**
 * The number of key value pairs currently stored in the table
 * @param cm Pointer to hash table
 * @return Number of elements stored in the hash table
 */
unsigned int cmap_count(const CMap *cm);

/**
 * @breif Inserts a key-value pair into the hash table
 * @param cm The CMap to insert a value into
 * @param key The key to insert
 * @param keysize The size of the key to insert
 * @param value The value to insert
 * @param valuesize The size of the value to insert
 * @return Pointer to the inserted key, if successfully inserted, othersie NULL.
 */
void *cmap_insert(CMap *cm, const void *key, const void *value);

/**
 * @breif looks up a key-value pair in in the hash table
 * @param cm The hash map to lookup the value in
 * @param key The key to lookup
 * @return Pointer to the value stored in the hash table. If there
 * is no such key in the hash table then NULL.
 */
void *cmap_lookup(const CMap *cm, const void *key);

/**
 * @breif Removes a key-value pair from the hash table
 * @param cm Hash table to remove the key value pair from
 * @param key The key to remove
 */
void cmap_remove(CMap *cm, const void *key);

/**
 * @breif Removes all of the elements from the hash tabls
 * @param cm The CMap to remove all the elements from
 */
void cmap_clear(CMap *cm);


const void * cmap_first(const CMap *cm);

/**
 * Gets a pointer to the next key
 * @param cm The hash table
 * @param prevkey The previous key
 * @return pointer to the next key, if there is one.
 */
const void * cmap_next(const CMap *cm, const void *prevkey);

#endif // _CMAP_H
