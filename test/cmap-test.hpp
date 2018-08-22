#ifndef LISP_CMAP_TEST_HPP
#define LISP_CMAP_TEST_HPP

#include <gtest/gtest.h>
#include <cmap.h>

#include <hash.h>
#include <permutations.h>

namespace {

  template <unsigned int N>
  unsigned int hash_to_n(const void *key, size_t keysize) {
    (void) key;
    (void) keysize;
    return N;
  }

  template <typename T, unsigned int N, unsigned int M, T Threshold>
  unsigned int two_hash(const void *key, size_t keysize) {
    (void) keysize;
    auto tp = static_cast<const T*>(key);
    return *tp >= Threshold ? N : M;
  };

  template <typename K, typename V>
  class MapTest : public testing::Test {
  protected:
    MapTest() : cm(nullptr) { }
    using Test::SetUp;
    void SetUp(CMapHashFn hash, CMapCmpFn cmp, unsigned int capacity_hint) {
      cm = cmap_create(sizeof(K), sizeof(V), hash, cmp, nullptr, nullptr, capacity_hint);
      ASSERT_FALSE(cm == nullptr);
    }
    void TearDown() { cmap_dispose(cm); }
    CMap *cm;
  };

  typedef MapTest<int, int> MapIntIntTest;
  TEST_F(MapIntIntTest, create) {
    SetUp(hash_to_n<0>, (CMapCmpFn) cmp_int, 10);
    EXPECT_EQ(cmap_count(cm), 0);
  }

  typedef MapTest<int, int> MapIntIntTest;
  TEST_F(MapIntIntTest, InsertOne) {
    SetUp(hash_to_n<0>, (CMapCmpFn) cmp_int, 10);

    int i = 42;
    cmap_insert(cm, &i, &i);
    EXPECT_EQ(cmap_count(cm), 1);
  }

  TEST_F(MapIntIntTest, Insert10) {
    SetUp(two_hash<int, 0, 100, 5>, (CMapCmpFn) cmp_int, 1024);

    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(cmap_count(cm), i);
      cmap_insert(cm, &i, &i);
    }

    // Make sure they're all in there
    for (int i = 0; i < 10; ++i) {
      const void *v = cmap_lookup(cm, &i);
      ASSERT_TRUE(v != nullptr);
      int value = *static_cast<const int*>(v);
      EXPECT_EQ(value, i);
    }
  }

  class PermutationMapTest : public testing::Test {
  protected:
    PermutationMapTest() : p(nullptr), cm(nullptr) { }
    using Test::SetUp;
    void SetUp(const char * str, CMapHashFn hash, CMapCmpFn cmp, unsigned int capacity_hint) {
      cm = cmap_create(sizeof(const char *), sizeof(int), hash, cmp, free, nullptr, capacity_hint);
      ASSERT_FALSE(cm == nullptr);
      p = new_cstring_permuter(str);
      ASSERT_FALSE(p == nullptr);
    }

    void TearDown() {
      cstring_permuter_dispose(p);
      cmap_dispose(cm);
    }
    permuter *p;
    CMap *cm;
  };

  TEST_F(PermutationMapTest, Permute1234) {
    SetUp("1234", murmur_hash, cmp_cstr, 1024);

    const void *permutation;
    for_permutations(p, permutation) {
      auto str = static_cast<const char *>(permutation);
      int i = permutation_index(p);
      char *s = strdup(str);
      cmap_insert(cm, &s, &i);
    }

    reset_permuter(p);

    // Check to make sure that they're all there
    for_permutations(p, permutation) {
      auto str = static_cast<const char *>(permutation);
      auto valuep = static_cast<int *>(cmap_lookup(cm, &str));
      ASSERT_TRUE(valuep != nullptr);
      EXPECT_EQ(*valuep, permutation_index(p));
    }
  }

  bool test_deletion(unsigned int capacity) {
    CMap *cm = cmap_create(sizeof(char *), sizeof(int),
                           string_hash, cmp_cstr,
                           NULL, NULL, capacity);

    if (cm == NULL) return false;

    if (cmap_count(cm) != 0)
      return false;

    // insert all number permutations into the cmap
    permuter *p1 = new_cstring_permuter("012345");
    const void *permutation;
    for_permutations(p1, permutation) {
      auto str = static_cast<const char *>(permutation);
      int i = permutation_index(p1);
      char *s = strdup(str);
      cmap_insert(cm, &s, &i);
    }

    // insert all letter permutations into the cmap
    permuter *p2 = new_cstring_permuter("abcdef");
    for_permutations(p2, permutation) {
      auto str = static_cast<const char *>(permutation);
      int i = permutation_index(p2);
      char *s = strdup(str);
      cmap_insert(cm, &s, &i);
    }

    reset_permuter(p1);
    reset_permuter(p2);

    // remove all the number permutations
    for_permutations(p1, permutation) {
      auto str = static_cast<const char *>(permutation);
      cmap_remove(cm, &str);
    }

    // check to make sure that all the letter permutations are there and correct
    for_permutations(p2, permutation) {
      auto str = static_cast<const char *>(permutation);
      auto valuep = static_cast<int *>(cmap_lookup(cm, &str));
      if (valuep == NULL)
        return false;

      if (*valuep != permutation_index(p2))
        return false;
    }

    cstring_permuter_dispose(p1);
    cstring_permuter_dispose(p2);

    cmap_dispose(cm);
    return true;
  }

  TEST(DeletionTest, LargeDeletion) {
    ASSERT_TRUE(test_deletion(1 << 21));
  }

}

#endif // LISP_CMAP_TEST_HPP