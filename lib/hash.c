#include "stdlib.h"
#include "string.h"
#include <murmur3.h>


unsigned int roberts_hash(const void *key, size_t keysize) {
  const unsigned long MULTIPLIER = 2630849305L;
  unsigned long hashcode = 0;
  for (int i = 0; i < (int) keysize; i++)
    hashcode = hashcode * MULTIPLIER + ((const char*) key)[i];
  return (unsigned int) hashcode;
}

unsigned int string_hash(const void *key, size_t keysize) {
  (void) keysize;
  const unsigned int MULTIPLIER = 2630849305;
  unsigned int hashcode = 0;
  const char *s = (char const *) key;
  for (int i = 0; s[i] != '\0'; i++)
    hashcode = hashcode * MULTIPLIER + s[i];
  return hashcode;
}

unsigned int murmur_hash(const void *key, size_t keysize) {
  unsigned int hash;
  MurmurHash3_x86_32(key, (int) keysize, 7, &hash);
  return hash;
}

unsigned int djb2_hash(unsigned char *str) {
  unsigned int hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

unsigned int sdbm(unsigned char *str) {
  unsigned int hash = 0;
  int c;

  while ((c = *str++))
    hash = c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

unsigned int loose_loose(unsigned char *str) {
  unsigned int hash = 0;
  int c;

  while ((c = *str++))
    hash += c;

  return hash;
}
