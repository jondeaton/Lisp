/**
 * @file cmap.h
 * @breif Defines the interface for the CMap type.
 */

#ifndef _CMAP_H
#define _CMAP_H

#define LOAD_FACTOR_LIMIT 0.9

#ifdef __cplusplus
#include <cstddef>
#include <cstdbool>
extern "C" {
#else

#include <stddef.h>
#include <stdbool.h>

#endif

#include <hash.h>
#include <ops.h>

/**
* @struct CMapImplementation
* @brief Definition of HashTable implementation
*/
struct Map {
  void *entries;                // Pointer to key-value pair array
  void *end;                    // End of buckets array
  unsigned int capacity;        // maximum number of values that can be stored
  unsigned int size;            // The number of elements stored in the hash table

  size_t key_size;              // The size of each key
  size_t value_size;            // The size of each value

  CleanupFn cleanupKey;         // Callback for key disposal
  CleanupFn cleanupValue;       // Callback for value disposal
  CMapHashFn hash;              // hash function callback
  CmpFn cmp;                    // key comparison function
};

typedef struct Map Map;

// macro for defining map of simple types that need no cleanup (i.e. int -> int)
#define simple_map(key_size, value_size) cmap_create(key_size, value_size, roberts_hash, NULL, NULL, NULL, 0)

/**
 * Create a HashTable in a dynamically allocated region of memory.
 * @param key_size size of all keys stored in HashTable
 * @param value_size size of all values stored in the HashTable
 * @param hash Hash function used to hash keys, may be NULL
 * @param cmp Comparison function between keys, may be NULL
 * @param cleanupKey Cleanup function for keys, may be NULL
 * @param cleanupValue Cleanup function for values for, may be NULL
 * @param capacity number of values that maybe stored in table
 * @return Pointer to a hash table in dynamically allocated memory, if
 * creation was successful otherwise NULL
 */
Map *new_cmap(size_t key_size, size_t value_size,
              CMapHashFn hash, CmpFn cmp,
              CleanupFn cleanupKey, CleanupFn cleanupValue,
              unsigned int capacity_hint);


/**
 * Initialize a new hash table
 * @param key_size size of all keys stored in HashTable
 * @param value_size size of all values stored in the HashTable
 * @param hash Hash function used to hash keys, may be NULL
 * @param cmp Comparison function between keys, may be NULL
 * @param cleanupKey Cleanup function for keys, may be NULL
 * @param cleanupValue Cleanup function for values for, may be NULL
 * @param capacity number of values that maybe stored in table
 * @return true if successful, false otherwise
 */
bool cmap_init(Map *cm, size_t key_size, size_t value_size,
               CMapHashFn hash, CmpFn cmp,
               CleanupFn cleanupKey, CleanupFn cleanupValue,
               unsigned int capacity);
/**
 * Dispose of a Hash Table initialized from cmap_init.
 * @note will not call free on the hash table. If you allocated
 * the hash table using new_cmap, then you should call free on it yourself
 * @param cm Pointer to hash table
 */
void cmap_dispose(Map *cm);

/**
 * The number of key value pairs currently stored in the table
 * @param cm Pointer to hash table
 * @return Number of elements stored in the hash table
 */
unsigned int cmap_count(const Map *cm);

/**
 * @breif Inserts a key-value pair into the hash table
 * @param cm The CMap to insert a value into
 * @param key The key to insert
 * @param keysize The size of the key to insert
 * @param value The value to insert
 * @param valuesize The size of the value to insert
 * @return Pointer to the inserted key, if successfully inserted, othersie NULL.
 */
void *cmap_insert(Map *cm, const void *key, const void *value);

/**
 * @breif looks up a key-value pair in in the hash table
 * @param cm The hash map to lookup the value in
 * @param key The key to lookup
 * @return Pointer to the value stored in the hash table. If there
 * is no such key in the hash table then NULL.
 */
void *cmap_lookup(const Map *cm, const void *key);

/**
 * @breif Removes a key-value pair from the hash table
 * @param cm Hash table to remove the key value pair from
 * @param key The key to remove
 */
void cmap_remove(Map *cm, const void *key);

/**
 * @breif Removes all of the elements from the hash tabls
 * @param cm The CMap to remove all the elements from
 */
void cmap_clear(Map *cm);

#define for_map_keys(cm, key) for ((key) = cmap_first(cm); \
                                   (key) != NULL; \
                                   (key) = cmap_next(cm, key))

/**
 * @brief Returns the value associated with a given key that was
 * returned by the cmap_first and cmap_next.
 * @note the key must have been returned by cmap_first or cmap_next
 * @param cm The cmap to get the value from
 * @param key Pointer to a key that is stored internally to the cmap
 * and was returned by cmap_first or cmap_next
 * @return Pointer to the value stored within the cmap which is
 * associated with the given key
 */
const void *get_value(const Map *cm, const void *key);

/**
 * @brief Get a reference to the first key-value pair stored in the
 * @param cm The cmap to get the first element of
 * @return Pointer to a key
 */
const void *cmap_first(const Map *cm);

/**
 * Gets a pointer to the next key, given some current key
 * @param cm The hash table
 * @param prevkey The previous key
 * @return pointer to the next key, if there is one.
 */
const void *cmap_next(const Map *cm, const void *prevkey);

#ifdef __cplusplus
}
#endif

#endif // _CMAP_H
