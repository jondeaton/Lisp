//
// Created by Jonathan Deaton on 7/18/18.
//

#include "stdlib.h"
#include "string.h"

unsigned int roberts_hash(const void *key, size_t keysize) {
  const unsigned long MULTIPLIER = 2630849305L; // magic number
  unsigned long hashcode = 0;
  for (int i = 0; i < (int) keysize; i++)
    hashcode = hashcode * MULTIPLIER + ((const char*) key)[i];
  return (unsigned int) hashcode;
}

unsigned int string_hash(const void *key, size_t keysize) {
  (void) keysize;
  const char *keystr = *(const char **) key;
  size_t keylen = strlen(keystr);
  return roberts_hash(keystr, keylen);
}

unsigned long djb2_hash(unsigned char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

unsigned long sdbm(unsigned char *str) {
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash = c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

unsigned long loose_loose(unsigned char *str) {
  unsigned int hash = 0;
  int c;

  while ((c = *str++))
    hash += c;

  return hash;
}
