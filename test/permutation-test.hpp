#ifndef LISP_PERMUTATION_TEST_HPP
#define LISP_PERMUTATION_TEST_HPP

#include <gtest/gtest.h>
#include <permutations.h>

template <class T>
class PermuterTest : public testing::Test {
protected:
  PermuterTest() : p(nullptr) { }
  using Test::SetUp;
  void SetUp(const T arr[], int nelems, CompareFn cmp) {
    _array = new T[nelems];
    memcpy(_array, arr, nelems * sizeof(T));
    p = new_permuter(_array, nelems, sizeof(T), cmp);
    ASSERT_FALSE(p == nullptr);
  }

  void TearDown() {
    delete[] _array;
    permuter_dispose(p);
  }

  permuter *p;
  T *_array;
};

class StringPermuterTest : public testing::Test {
protected:
  StringPermuterTest() : p(nullptr) { }
  using Test::SetUp;
  void SetUp(const char *str) {
    p = new_cstring_permuter(str);
    ASSERT_FALSE(p == nullptr);
  }

   void TearDown() {
    cstring_permuter_dispose(p);
  }

  permuter *p;
};

namespace {

  // Tests factorial of negative numbers.
  TEST(FactorialTest, Negative) {
    EXPECT_EQ(1, factorial(-5));
    EXPECT_EQ(1, factorial(-1));
    EXPECT_GT(factorial(-10), 0);
  }

  // Tests factorial of 0.
  TEST(FactorialTest, Zero) {
    EXPECT_EQ(1, factorial(0));
  }

  // Tests factorial of positive numbers.
  TEST(FactorialTest, Positive) {
    EXPECT_EQ(1, factorial(1));
    EXPECT_EQ(2, factorial(2));
    EXPECT_EQ(6, factorial(3));
    EXPECT_EQ(40320, factorial(8));
  }

  // empty string permutation
  TEST_F(StringPermuterTest, EmptyString) {
    SetUp("");
    EXPECT_EQ(permutation_index(p), 0);

    void *permutation = get_permutation(p);
    auto str = static_cast<const char*>(permutation);
    EXPECT_STREQ(str, "");

    EXPECT_EQ(next_permutation(p), nullptr);
    EXPECT_EQ(permutation_index(p), 0);

    // make sure it didn't change
    EXPECT_STREQ(str, "");
  }

  // Tests explicitly for all permutations of one character
  TEST_F(StringPermuterTest, One) {
    SetUp("1");
    const char* correct_permutation[] = { "1" };

    for (int repeat = 0; repeat < 2; repeat++) {

      void *permutation;
      for_permutations(p, permutation)  {
        auto str = static_cast<const char*>(permutation);
        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_STREQ(str, correct_permutation[i]);
      }

      ASSERT_EQ(next_permutation(p), nullptr);
      reset_permuter(p);
    }
  }

  // Tests explicitly for all permutations of two characters
  TEST_F(StringPermuterTest, Two) {
    SetUp("12");
    const char* correct_permutation[] = { "12", "21" };

    for (int repeat = 0; repeat < 3; repeat++) {
      void *permutation;
      for_permutations(p, permutation)  {
        auto str = static_cast<const char*>(permutation);
        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_STREQ(str, correct_permutation[i]);
      }

      ASSERT_EQ(next_permutation(p), nullptr);
      reset_permuter(p);
    }
  }

  // Tests explicitly for all permutations of three characters
  TEST_F(StringPermuterTest, Three) {
    SetUp("123");
    const char* correct_permutation[] = { "123", "132", "312",
                                          "321", "231", "213" };

    for (int repeat = 0; repeat < 3; repeat++) {
      void *permutation;
      for_permutations(p, permutation)  {
        auto str = static_cast<const char*>(permutation);
        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_STREQ(str, correct_permutation[i]);
      }

      ASSERT_EQ(next_permutation(p), nullptr);
      reset_permuter(p);
    }
  }

  // Compare void * pointers to the same type of value
  template <typename T>
  static inline int cmp(const void *Tptr1, const void *Tptr2) {
    auto t1 = static_cast<const T*>(Tptr1);
    auto t2 = static_cast<const T*>(Tptr2);
    return *t1 - *t2;
  }

  // Compare two arrays of the same type of elements
  template <typename T, int N>
  static inline bool arr_eq(const T arr1[N], const T arr2[N]) {
    for (int i = 0; i < N; i++)
      if (arr1[i] > arr2[i]) return false;
    return true;
  }

  typedef PermuterTest<int> IntegerPermuterTest;
  TEST_F(IntegerPermuterTest, Three) {
    const int arr[] = {1, 2, 3};
    SetUp(arr, 3, cmp<int>);
    const int correct_permutations[6][3] =
      {
        {1, 2, 3}, {1, 3, 2}, {3, 1, 2},
        {3, 2, 1}, {2, 3, 1}, {2, 1, 3}
      };

    for (int repeat = 0; repeat < 2; repeat++) {
      void *permutation;
      int index = 0;
      for_permutations(p, permutation)  {
        ASSERT_TRUE(permutation != NULL);

        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_EQ(i, index);
        bool equal = arr_eq<int, 3>(correct_permutations[i],
                                    static_cast<const int*>(permutation));
        EXPECT_TRUE(equal);
        index++;
      }

      ASSERT_EQ(next_permutation(p), nullptr);
      reset_permuter(p);
    }
  }

  TEST_F(StringPermuterTest, EightCount) {
    const char *s = "12345678";
    SetUp(s);
    for (int repeat = 0; repeat < 2; repeat++) {
      int num_permutations = 0;
      void *permutation;
      for_permutations(p, permutation)
        num_permutations++;
      EXPECT_EQ(num_permutations, factorial(strlen(s)));
      reset_permuter(p);
    }
  }

  // Make sure that 9! permutations happen
  TEST_F(StringPermuterTest, NineCount) {
    const char *s = "123456789";
    SetUp(s);
    for (int repeat = 0; repeat < 2; repeat++) {
      int num_permutations = 0;
      void *permutation;
      for_permutations(p, permutation)
        num_permutations++;
      EXPECT_EQ(num_permutations, factorial(strlen(s)));
      reset_permuter(p);
    }
  }

}


#endif //LISP_PERMUTATION_TEST_HPP
