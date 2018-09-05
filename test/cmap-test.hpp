#ifndef LISP_CMAP_TEST_HPP
#define LISP_CMAP_TEST_HPP

#include <gtest/gtest.h>
#include <cmap.h>

#include <hash.h>
#include <permutations.h>
#include <ops.h>

namespace {

  /**
   * Function that "hashes" everything to the same value!
   * (very useful for testing the behavior of hash collisions in a hash table)
   * @tparam N what to hash elements to
   * @param key the key to hash (unused)
   * @param keysize size of the key (unused)
   * @return the value of N (always!)
   */
  template <unsigned int N>
  constexpr unsigned int hash_to_n(const void *key UNUSED, size_t keysize UNUSED) {
    return N;
  }

  /**
   * Function that "hashes" everything to one of two values!
   * Again, very helpful for testing hash collision behavior, especially with open addressing
   * @tparam T The type of the key (make sure it supports GTE operator)
   * @tparam N first value that might be hashed to
   * @tparam M second value that might be hashed to
   * @tparam Threshold threshold for key value above which this function returns M rather than N
   * @param key pointer to the key to hash
   * @param keysize The size of the key (unused since keysize can be had from T)
   * @return N if the key is at least the Threshold, otherwise M
   */
  template <typename T, unsigned int N, unsigned int M, T Threshold>
  unsigned int two_hash(const void *key, size_t keysize UNUSED) {
    auto tp = static_cast<const T*>(key);
    return *tp >= Threshold ? N : M;
  }

  template <typename K, typename V>
  class MapTest : public testing::Test {
  protected:
    MapTest() : cm(nullptr) { }
    using Test::SetUp;
    void SetUp(CMapHashFn hash, CmpFn cmp, unsigned int capacity_hint) {
      cm = cmap_create(sizeof(K), sizeof(V), hash, cmp, nullptr, nullptr, capacity_hint);
      ASSERT_NE(cm, nullptr);
    }
    void TearDown() { cmap_dispose(cm); }
    CMap *cm;
  };

  typedef MapTest<int, int> MapIntIntTest;
  TEST_F(MapIntIntTest, create) {
    SetUp(hash_to_n<0>, (CmpFn) cmp_int, 0);
    EXPECT_EQ(cmap_count(cm), 0);
  }

  typedef MapTest<int, int> MapIntIntTest;
  TEST_F(MapIntIntTest, InsertOne) {
    SetUp(hash_to_n<0>, (CmpFn) cmp_int, 0);

    int i = 42;
    cmap_insert(cm, &i, &i);
    EXPECT_EQ(cmap_count(cm), 1);
  }

  TEST_F(MapIntIntTest, Insert10) {
    SetUp(two_hash<int, 0, 100, 5>, (CmpFn) cmp_int, 0);

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

  // test if things are inserted correctly when one collection of elements overlaps with another set
  TEST_F(MapIntIntTest, InsertOverlap) {
    constexpr int first_hash = 0;
    constexpr int second_hash = 10;
    constexpr int threshold = 1000;
    SetUp(two_hash<int, first_hash, second_hash, threshold>, (CmpFn) cmp_int, 0);

    constexpr int N = 2 * second_hash; // how many of each hash to insert

    const int first_value = 42;
    const int second_value = 314;

    // insert elements hashing to first location, enough to cover up the origin of the
    // next set of elements
    for (int i = 0; i < N; i++)
      cmap_insert(cm, &i, &first_value);

    // these ones will all hash to "second_hash"
    for (int i = threshold; i < threshold + N; i++)
      cmap_insert(cm, &i, &second_value);

    for (int i = N; i < 2 * N; i++)
      cmap_insert(cm, &i, &first_value);

    const int *valuep;
    for (int i = 0; i < 2 * N; i++) {
      valuep = static_cast<const int *>(cmap_lookup(cm, &i));
      ASSERT_TRUE(valuep != nullptr);
      EXPECT_EQ(*valuep, first_value);
    }

    for (int i = threshold; i < threshold + N; i++) {
      valuep = static_cast<const int *>(cmap_lookup(cm, &i));
      ASSERT_TRUE(valuep != nullptr);
      EXPECT_EQ(*valuep, second_value);
    }
  }

  TEST_F(MapIntIntTest, InsertStaggeredOverlap) {
    constexpr int first_hash = 0;
    constexpr int second_hash = 10;
    constexpr int threshold = 1000;
    SetUp(two_hash<int, first_hash, second_hash, threshold>, (CmpFn) cmp_int, 0);

    constexpr int N = 2 * second_hash; // how many of each hash to insert
    const int first_value = 42;
    const int second_value = 314;

    // insert a bunch of elements near each-other until they start overlapping with one another
    // at which point they will need to stagger
    for (int i = 0; i < N; i++) {
      cmap_insert(cm, &i, &first_value);
      const int second_key = threshold + i;
      cmap_insert(cm, &second_key, &second_value);
    }

    // make sure they're all in there
    for (int i = 0; i < N; i++) {
      auto valuep = static_cast<const int *>(cmap_lookup(cm, &i));
      ASSERT_NE(valuep, nullptr);
      EXPECT_EQ(*valuep, first_value);

      const int second_key = threshold + i;
      valuep = static_cast<const int *>(cmap_lookup(cm, &second_key));
      ASSERT_NE(valuep, nullptr);
      EXPECT_EQ(*valuep, second_value);
    }
  }

  // Make sure that when you delete an element, its gone.
  TEST_F(MapIntIntTest, Delete) {
    SetUp(two_hash<int, 0, 100, 5>, (CmpFn) cmp_int, 0);
    int i = 42;
    cmap_insert(cm, &i, &i);
    cmap_remove(cm, &i);
    EXPECT_EQ(cmap_lookup(cm, &i), nullptr);
  }

  // Make sure that when you delete an element when there was a collision, the other
  // one is still there
  TEST_F(MapIntIntTest, DeleteWithCollision) {
    SetUp(two_hash<int, 0, 100, 5>, (CmpFn) cmp_int, 0);

    int x = 0;
    int y = 1;
    cmap_insert(cm, &x, &x);
    cmap_insert(cm, &y, &y);

    cmap_remove(cm, &x);

    EXPECT_EQ(cmap_lookup(cm, &x), nullptr);

    auto yp = static_cast<const int*>(cmap_lookup(cm, &y));
    EXPECT_EQ(*yp, y);
  }

  TEST_F(MapIntIntTest, DeleteSecondWithCollision) {
    SetUp(two_hash<int, 0, 100, 5>, (CmpFn) cmp_int, 0);

    int x = 0;
    int y = 1;
    cmap_insert(cm, &x, &x);
    cmap_insert(cm, &y, &y);

    cmap_remove(cm, &y);
    EXPECT_EQ(cmap_lookup(cm, &y), nullptr);

    auto xp = static_cast<const int*>(cmap_lookup(cm, &x));
    EXPECT_EQ(*xp, x);
  }

  TEST_F(MapIntIntTest, DeleteOverlap) {
    constexpr int first_hash = 0;
    constexpr int second_hash = 10;
    constexpr int threshold = 1000;
    SetUp(two_hash<int, first_hash, second_hash, threshold>, (CmpFn) cmp_int, 0);

    constexpr int N = 2 * second_hash; // how many of each hash to insert
    const int first_value = 42;
    const int second_value = 314;

    // insert a bunch of elements near each-other until they start overlapping with one another
    // at which point they will need to stagger
    for (int i = 0; i < N; i++) {
      cmap_insert(cm, &i, &first_value);
      const int second_key = threshold + i;
      cmap_insert(cm, &second_key, &second_value);
    }

    // delete all the ones from the lower hash value that overlapped into the second hash
    for (int i = 0; i < N; i++)
      cmap_remove(cm, &i);

    // make sure all the higher hash value elements are sill there
    for (int i = threshold; i < N; i++) {
      auto vp = static_cast<const int *>(cmap_lookup(cm, &i));
      ASSERT_NE(vp, nullptr);
      EXPECT_EQ(*vp, second_value);
    }
  }

  // the next few tests use the permuter library to insert a whole bunch
  // of elements. this doesn't test for anything in particular but just
  // hopefully might catch something wrong that wasn't tested for in other cases
  class PermutationMapTest : public testing::Test {
  protected:
    PermutationMapTest() : p(nullptr), cm(nullptr) { }
    using Test::SetUp;
    void SetUp(const char * str, CMapHashFn hash, CmpFn cmp, unsigned int capacity_hint) {
      cm = cmap_create(sizeof(const char *), sizeof(int), hash, cmp, str_cleanup, nullptr, capacity_hint);
      ASSERT_NE(cm, nullptr);
      p = new_cstring_permuter(str);
      ASSERT_NE(p, nullptr);
    }

    // cleans up one of the string keys (which has been malloc'd)
    static void str_cleanup(void *key) {
      char *s = *(char **) key;
      free(s);
    }

    void TearDown() {
      cstring_permuter_dispose(p);
      cmap_dispose(cm);
    }
    permuter *p;
    CMap *cm;
  };

  TEST_F(PermutationMapTest, Permute1234) {
    SetUp("1234", string_hash, cmp_cstr, 1024);

    const void *permutation;
    for_permutations(p, permutation) {
      auto str = static_cast<const char *>(permutation);
      int i = permutation_index(p);
      char *s = strdup(str);
      cmap_insert(cm, &s, &i);
      EXPECT_EQ(cmap_count(cm), i + 1);
    }

    reset_permuter(p);

    // Check to make sure that they're all there
    for_permutations(p, permutation) {
      auto str = static_cast<const char *>(permutation);
      auto valuep = static_cast<int *>(cmap_lookup(cm, &str));
      ASSERT_NE(valuep, nullptr);
      EXPECT_EQ(*valuep, permutation_index(p));
    }
  }
}

#endif // LISP_CMAP_TEST_HPP
