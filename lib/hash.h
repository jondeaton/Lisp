#ifndef _hashtable_hash_h
#define _hashtable_hash_h

typedef unsigned int (*CMapHashFn)(const void *key, size_t keysize);

unsigned int roberts_hash(const void *key, size_t keysize);
unsigned int string_hash(const void *key, size_t keysize);
unsigned int murmur_hash(const void *key, size_t keysize);
unsigned int djb2_hash(unsigned char *str);
unsigned int sdbm(unsigned char *str);
unsigned int loose_loose(unsigned char *str);

#endif // _hashtable_hash_h
