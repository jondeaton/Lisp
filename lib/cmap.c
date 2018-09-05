/**
 * @file cmap.c
 * @brief Defines the implementation of a HashTable in C.
 * This implementation uses an open-addressing scheme.
 * There are future plans to use robin-hood scheme.
 */

#include "cmap.h"
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// a suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 1024

/**
 * @struct CMapImplementation
 * @brief Definition of HashTable implementation
 */
struct CMapImplementation {
  void *entries;                // Pointer to key-value pair array
  void *end;                    // End of buckets array
  unsigned int capacity;        // maximum number of values that can be stored
  unsigned int size;            // The number of elements stored in the hash table

  size_t key_size;              // The size of each key
  size_t value_size;            // The size of each value

  CleanupFn cleanupKey;         // Callback for key disposal
  CleanupFn cleanupValue;       // Callback for value disposal
  CMapHashFn hash;              // hash function callback
  CmpFn cmp;                // key comparison function
};

/**
 * @struct entry
 * Stores metadata about a single key-value pair
 */
struct entry {
  unsigned int hash;        // hash of key
  uint8_t status;           // status bits
  char kv[];                // Key/value pair
};

// Macros/functions for setting entry status bits
#define FREE_MASK ((uint8_t) 1)

// read the "free bit" from the status bits in the entry
static inline bool is_free(const struct entry* e) {
  return (bool) (e->status & FREE_MASK);
}

// set the "free bit" in the status bits in the entry
static inline void set_free(struct entry *e, bool free) {
  if (free) e->status |= FREE_MASK;
  else e->status &= ~FREE_MASK;
}

// static function declarations
static inline struct entry *entry_of(const void *key);
static inline size_t entry_size(const CMap *cm);
static inline struct entry *get_entry(const CMap *cm, unsigned int index);
static struct entry *lookup_key(const CMap *cm, const void *key);
static inline void *value_of(const CMap *cm, const struct entry *entry);
static inline void *key_of(const struct entry *entry);
static inline void move(CMap *cm, struct entry *entry1, struct entry *entry2);
static void erase(CMap *cm, struct entry *e);
static void delete(CMap *cm, unsigned int start, unsigned int stop);
static int lookup_index(const CMap *cm, const void *key);

CMap *cmap_create(size_t key_size, size_t value_size,
                  CMapHashFn hash, CmpFn cmp,
                  CleanupFn cleanupKey, CleanupFn cleanupValue,
                  unsigned int capacity) {
  if (key_size <= 0 || value_size <= 0) return NULL;

  CMap* cm = malloc(sizeof(CMap));
  if (cm == NULL) return NULL;

  cm->key_size = key_size;
  cm->value_size = value_size;
  cm->size = 0;
  cm->cleanupKey = cleanupKey;
  cm->cleanupValue = cleanupValue;
  cm->hash = hash == NULL ? roberts_hash : hash;
  cm->cmp = cmp == NULL ? memcmp : cmp;

  // Allocate array for key-value entries
  cm->capacity = capacity > 0 ? capacity : DEFAULT_CAPACITY;
  cm->entries = malloc(cm->capacity * entry_size(cm));
  if (cm->entries == NULL) {
    free(cm); // wouldn't wanna leak memory while running out of it eh?
    return NULL;
  }

  // Set all the entries to free
  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, i);
    assert(e != NULL);
    set_free(e, true);
  }

  return cm;
}

void cmap_dispose(CMap* cm) {
  assert(cm != NULL);
  cmap_clear(cm);
  free(cm->entries);
  free(cm);
}

unsigned int cmap_count(const CMap* cm) {
  assert(cm != NULL);
  return cm->size;
}

void *cmap_insert(CMap *cm, const void *key, const void *value) {
  assert(cm != NULL);
  assert(key != NULL);
  assert(value != NULL);

  // there is no vacancy
  if (cm->size == cm->capacity)
    return NULL;

  unsigned int hash = cm->hash(key, cm->key_size) % cm->capacity;

  // Locate a free entry (guaranteed to exist)
  struct entry *entry = NULL;
  for (unsigned int i = 0; i < cm->capacity; ++i) {
    entry = get_entry(cm, (hash + i) % cm->capacity);
    if (is_free(entry)) break;
  }

  assert(entry != NULL);

  // Fill the entry with the key-value pair
  memcpy(key_of(entry), key, cm->key_size);
  memcpy(value_of(cm, entry), value, cm->value_size);

  set_free(entry, false);
  entry->hash = hash;

  cm->size++;
  return entry;
}

void *cmap_lookup(const CMap *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);

  struct entry *entry = lookup_key(cm, key);
  if (entry == NULL) return NULL;
  return value_of(cm, entry);
}

void cmap_remove(CMap *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);

  int start = lookup_index(cm, key);
  if (start < 0) return;
  struct entry *e = get_entry(cm, start);
  assert(e != NULL);

  erase(cm, e);
  delete(cm, start, e->hash % cm->capacity);
  cm->size--;
}

void cmap_clear(CMap *cm) {
  assert(cm != NULL);

  unsigned int num_cleared = 0;
  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, i);
    assert(e != NULL);
    if (is_free(e)) continue;

    erase(cm, e);
    num_cleared++;

    // If we've deleted them all, stop and save a bit of searching
    if (num_cleared == cm->size) break;
  }
  cm->size = 0;
}

const void *get_value(const CMap *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);
  return value_of(cm, key);
}

const void *cmap_first(const CMap *cm) {
  assert(cm != NULL);

  if (cm == NULL) return NULL;
  if (cm->size == 0) return NULL;

  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, i);
    if (!is_free(e)) return key_of(e);
  }
  return NULL;
}

const void *cmap_next(const CMap *cm, const void *prevkey) {
  assert(cm != NULL);
  assert(prevkey != NULL);

  const struct entry *e = entry_of(prevkey);
  while (e != cm->end) {
    e = (struct entry *) ((char *) e + entry_size(cm));
    if (!is_free(e)) return key_of(e);
  }
  return NULL;
}

// Gives the entry for a given key
static inline struct entry *entry_of(const void *key) {
  assert(key != NULL);
  return (struct entry *) ((char *) key - offsetof(struct entry, kv));
}

static inline size_t entry_size(const CMap *cm) {
  assert(cm != NULL);
  return sizeof(struct entry) + cm->key_size + cm->value_size;
}

static inline struct entry *get_entry(const CMap *cm, unsigned int index) {
  assert(cm != NULL);
  assert(index < cm->capacity);
  void *entry = (char *) cm->entries + index * entry_size(cm);
  return (struct entry *) entry;
}

/**
 * @breif Finds the entry for this key
 * @param cm The CMap to lookup the key in
 * @param key the key to lookup in the CMap
 * @return pointer to the hash table entry that contains the key and value
 * if the key exists in the hash table, else NULL
 */
static struct entry *lookup_key(const CMap *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);
  if (cm->size == 0) return NULL;

  unsigned int hash = cm->hash(key, cm->key_size) % cm->capacity;
  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, (hash + i) % cm->capacity);
    if (is_free(e)) return NULL;

    // use cached hash value to do an easy/cache-friendly comparison
    if (e->hash != hash) continue;

    // only dereference to compare full keys if you have to
    if (cm->cmp(&e->kv, key, cm->key_size) == 0)
      return e;
  }
  return NULL; // Went all the way around
}

static inline void *value_of(const CMap *cm, const struct entry *entry) {
  assert(cm != NULL);
  assert(entry != NULL);
  return (char *) &entry->kv + cm->key_size;
}

static inline void *key_of(const struct entry *entry) {
  assert(entry != NULL);
  return (void *) (&entry->kv);
}

static inline void move(CMap *cm, struct entry *entry1, struct entry *entry2) {
  assert(cm != NULL);
  assert(entry1 != NULL);
  assert(entry2 != NULL);
  memcpy(entry1, entry2, entry_size(cm));
}

static void erase(CMap *cm, struct entry *e) {
  assert(cm != NULL);
  assert(e != NULL);

  if (cm->cleanupKey != NULL)
    cm->cleanupKey(key_of(e));

  if (cm->cleanupValue != NULL)
    cm->cleanupValue(value_of(cm, e));

  set_free(e, true);
}

static void delete(CMap *cm, unsigned int start, unsigned int stop) {
  assert(cm != NULL);
  while (true) {
    // the entry to delete
    struct entry *entry = get_entry(cm, start);

    unsigned int j = start;
    for (unsigned int i = 0; i < cm->capacity; ++i) {
      j = (j + 1) % cm->capacity;
      if (j == stop) return;

      struct entry *next = get_entry(cm, j);
      if (is_free(next)) return; // reached the end

      // Found one that can replace
      if ((next->hash) % cm->capacity >= start) {
        move(cm, entry, next);
        set_free(next, true);
        break;
      }
    }
    start = j;
  }
}

static int lookup_index(const CMap *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);

  unsigned int hash = cm->hash(key, cm->key_size) % cm->capacity;

  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, (hash + i) % cm->capacity);
    if (e == NULL || is_free(e)) return -1;

    // Use cached hash value to do an easy/cache-friendly comparison
    if (e->hash != hash) continue;

    // Only dereference to compare full keys if you have to
    if (cm->cmp(&e->kv, key, cm->key_size) == 0)
      return (hash + i) % cm->capacity;
  }
  return -1;
}
