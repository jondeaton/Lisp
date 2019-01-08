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
#define DEFAULT_HASH murmur_hash

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
static inline size_t entry_size(const Map *cm);
static inline struct entry *get_entry(const Map *cm, unsigned int index);
static struct entry *lookup_key(const Map *cm, const void *key);
static inline void *value_of(const Map *cm, const struct entry *entry);
static inline void *key_of(const struct entry *entry);
static inline void move(Map *cm, struct entry *entry1, struct entry *entry2);
static void erase(Map *cm, struct entry *e);
static void delete(Map *cm, unsigned int start, unsigned int stop);
static int lookup_index(const Map *cm, const void *key);
static int compare(const Map *cm, const void *keyA, const void *keyB);
static bool expand_rehash(Map *cm);
static inline float load_factor(unsigned int count, unsigned int capacity);
static inline bool allocate_entries(Map *cm, unsigned int capacity);
static void set_entries_free(Map *cm);

Map *new_cmap(size_t key_size, size_t value_size,
              CMapHashFn hash, CmpFn cmp,
              CleanupFn cleanupKey, CleanupFn cleanupValue,
              unsigned int capacity) {

  Map* cm = malloc(sizeof(Map));
  if (cm == NULL) return NULL;

  bool success = cmap_init(cm, key_size, value_size, hash, cmp,
                           cleanupKey, cleanupValue, capacity);

  if (!success) {
    free(cm);
    return false;
  }

  return cm;
}

bool cmap_init(Map *cm, size_t key_size, size_t value_size,
               CMapHashFn hash, CmpFn cmp,
               CleanupFn cleanupKey, CleanupFn cleanupValue,
               unsigned int capacity) {
  assert(cm != NULL);

  cm->key_size = key_size;
  cm->value_size = value_size;
  cm->size = 0;
  cm->cleanupKey = cleanupKey;
  cm->cleanupValue = cleanupValue;
  cm->hash = hash == NULL ? DEFAULT_HASH : hash;
  cm->cmp = cmp;

  bool success = allocate_entries(cm, capacity > 0 ? capacity : DEFAULT_CAPACITY);
  if (!success) return false;
  return true;
}

void cmap_dispose(Map* cm) {
  assert(cm != NULL);
  cmap_clear(cm);
  free(cm->entries);
}

unsigned int cmap_count(const Map* cm) {
  assert(cm != NULL);
  return cm->size;
}

void *cmap_insert(Map *cm, const void *key, const void *value) {
  assert(cm != NULL);
  assert(key != NULL);
  assert(value != NULL);

  if (load_factor(cm->size + 1, cm->capacity) >= LOAD_FACTOR_LIMIT) {
    bool success = expand_rehash(cm);
    if (!success) return NULL;
  }

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

void *cmap_lookup(const Map *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);

  struct entry *entry = lookup_key(cm, key);
  if (entry == NULL) return NULL;
  return value_of(cm, entry);
}

void cmap_remove(Map *cm, const void *key) {
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

void cmap_clear(Map *cm) {
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

const void *get_value(const Map *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);
  return value_of(cm, key);
}

const void *cmap_first(const Map *cm) {
  assert(cm != NULL);

  if (cm->size == 0) return NULL;

  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, i);
    if (!is_free(e)) return key_of(e);
  }
  return NULL;
}

const void *cmap_next(const Map *cm, const void *prevkey) {
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

static inline size_t entry_size(const Map *cm) {
  assert(cm != NULL);
  return sizeof(struct entry) + cm->key_size + cm->value_size;
}

static inline struct entry *get_entry(const Map *cm, unsigned int index) {
  assert(cm != NULL);
  assert(index <= cm->capacity);
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
static struct entry *lookup_key(const Map *cm, const void *key) {
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
    if (compare(cm, &e->kv, key) == 0)
      return e;
  }
  return NULL; // Went all the way around
}

static inline void *value_of(const Map *cm, const struct entry *entry) {
  assert(cm != NULL);
  assert(entry != NULL);
  return (char *) &entry->kv + cm->key_size;
}

static inline void *key_of(const struct entry *entry) {
  assert(entry != NULL);
  return (void *) (&entry->kv);
}

static inline void move(Map *cm, struct entry *entry1, struct entry *entry2) {
  assert(cm != NULL);
  assert(entry1 != NULL);
  assert(entry2 != NULL);
  memcpy(entry1, entry2, entry_size(cm));
}

static void erase(Map *cm, struct entry *e) {
  assert(cm != NULL);
  assert(e != NULL);

  if (cm->cleanupKey != NULL)
    cm->cleanupKey(key_of(e));

  if (cm->cleanupValue != NULL)
    cm->cleanupValue(value_of(cm, e));

  set_free(e, true);
}

static void delete(Map *cm, unsigned int start, unsigned int stop) {
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

static int lookup_index(const Map *cm, const void *key) {
  assert(cm != NULL);
  assert(key != NULL);

  unsigned int hash = cm->hash(key, cm->key_size) % cm->capacity;

  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, (hash + i) % cm->capacity);
    if (e == NULL || is_free(e)) return -1;

    // Use cached hash value to do an easy/cache-friendly comparison
    if (e->hash != hash) continue;

    // Only dereference to compare full keys if you have to
    if (compare(cm, &e->kv, key) == 0)
      return (hash + i) % cm->capacity;
  }
  return -1;
}

static int compare(const Map *cm, const void *keyA, const void *keyB) {
  assert(cm != NULL);
  assert(keyA != NULL);
  assert(keyB != NULL);
  if (cm->cmp == NULL) return memcmp(keyA, keyB, cm->key_size);
  return cm->cmp(keyA, keyB);
}

static bool expand_rehash(Map *cm) {
  assert(cm != NULL);

  // expand
  struct entry *old_entries = cm->entries;
  unsigned int old_capacity = cm->capacity;
  unsigned int new_capacity = old_capacity > 0 ? 2 * old_capacity : DEFAULT_CAPACITY;

  bool success = allocate_entries(cm, new_capacity);
  if (!success) return false;

  cm->size = 0;

  // rehash
  struct entry *e = old_entries;
  for (unsigned int i = 0; i < old_capacity; ++i) {

    if (!is_free(e)) {
      const void *key = key_of(e);
      const void *value = value_of(cm, e);
      cmap_insert(cm, key, value);
    }

    e = (struct entry *) ((char *) e + entry_size(cm));
  }

  return true;
}

static inline float load_factor(unsigned int count, unsigned int capacity) {
  if (capacity == 0) return 1;
  return count / ((float) capacity);
}

static inline bool allocate_entries(Map *cm, unsigned int capacity) {
  assert(cm != NULL);
  struct entry *new_entries = malloc(capacity * entry_size(cm));
  if (new_entries == NULL) return false;

  // important not to change entries in the case of failure
  cm->entries = new_entries;
  cm->capacity = capacity;
  cm->end = get_entry(cm, capacity);

  set_entries_free(cm);
  return true;
}

// Set all the entries to free
static void set_entries_free(Map *cm) {
  assert(cm != NULL);
  for (unsigned int i = 0; i < cm->capacity; ++i) {
    struct entry *e = get_entry(cm, i);
    assert(e != NULL);
    set_free(e, true);
  }
}
