#ifndef _hashtable_hash_h
#define _hashtable_hash_h

/**
 * @breif default hash function for hash table
 * @detail This function adapted from Eric Roberts' _The Art and Science of C_
 * The hash code is computed using a method called "linear congruence."
 * A similar function using this method is described on page 144 of Kernighan
 * and Ritchie.
 */
unsigned int roberts_hash(const void *key, size_t keysize);

// example of a string hash function that could be used for string keys (string'ies)
unsigned int string_hash(const void *key, size_t keysize);

unsigned long djb2_hash(unsigned char *str);
unsigned long sdbm(unsigned char *str);
unsigned long loose_loose(unsigned char *str);

#endif // _hashtable_hash_h
