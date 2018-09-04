#ifndef PERMUTATION_TEST_HPP
#define PERMUTATION_TEST_HPP

#include <gtest/gtest.h>
#include <permutations.h>

namespace {

  /**
   * @class PermuterTest
   * @brief sets-up and tears down a permuter object. Also copies the elements
   * passed in so that they can be mutated in writable memory
   * @tparam T the type of element in the array of elements to permute
   */
  template<class T>
  class PermuterTest : public testing::Test {
  protected:
    PermuterTest() : p(nullptr) {}

    using Test::SetUp;

    void SetUp(const T arr[], int nelems, CompareFn cmp) {
      // need to make a copy of the passed array because we want
      // to use array-literals in read-only memory during testing
      _arr_cpy = new T[nelems];
      memcpy(_arr_cpy, arr, nelems * sizeof(T));
      p = new_permuter(_arr_cpy, nelems, sizeof(T), cmp);
      ASSERT_FALSE(p == nullptr);
    }

    void TearDown() {
      delete[] _arr_cpy;
      permuter_dispose(p);
    }

    permuter *p;
    T *_arr_cpy;
  };

  /**
   * @class StringPermuterTest
   * @brief sets up and tears down a string permuter object
   * for testing permutations of C-strings
   */
  class StringPermuterTest : public testing::Test {
  protected:
    StringPermuterTest() : p(nullptr) {}

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
    auto str = static_cast<const char *>(permutation);
    EXPECT_STREQ(str, "");

    EXPECT_EQ(next_permutation(p), nullptr);
    EXPECT_EQ(permutation_index(p), 0);

    // make sure it didn't change
    EXPECT_STREQ(str, "");
  }

  // Tests explicitly for all permutations of one character
  TEST_F(StringPermuterTest, One) {
    SetUp("1");
    const char *correct_permutation[] = {"1"};

    for (int repeat = 0; repeat < 2; repeat++) {

      void *permutation;
      for_permutations(p, permutation) {
        auto str = static_cast<const char *>(permutation);
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
    const char *correct_permutation[] = {"12", "21"};

    for (int repeat = 0; repeat < 3; repeat++) {
      void *permutation;
      for_permutations(p, permutation) {
        auto str = static_cast<const char *>(permutation);
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
    const char *correct_permutation[] = {"123", "132", "312",
                                         "321", "231", "213"};

    for (int repeat = 0; repeat < 3; repeat++) {
      void *permutation;
      for_permutations(p, permutation) {
        auto str = static_cast<const char *>(permutation);
        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_STREQ(str, correct_permutation[i]);
      }

      ASSERT_EQ(next_permutation(p), nullptr);
      reset_permuter(p);
    }
  }

  // Compare two void * pointers to the same type of value
  template<typename T>
  inline int cmp(const void *Tptr1, const void *Tptr2) {
    assert(Tptr1 != NULL);
    assert(Tptr2 != NULL);
    auto t1 = static_cast<const T *>(Tptr1);
    auto t2 = static_cast<const T *>(Tptr2);
    return *t1 - *t2;
  }

  // Compare two arrays of the same type of elements
  template<typename T, int N>
  inline bool arr_eq(const T arr1[N], const T arr2[N]) {
    for (int i = 0; i < N; i++)
      if (arr1[i] > arr2[i]) return false;
    return true;
  }

  // Did I create this entire permutation library and test suite
  // just to fabricate a use-case for the cannonical "factorial"
  // function using template metaprogramming? You'll never know...
  template<int N>
  struct Factorial {
    enum {
      value = N * Factorial<N - 1>::value
    };
  };

  template<>
  struct Factorial<0> {
    enum {
      value = 1
    };
  };

  // Tests that permutations of integers will work correctly
  typedef PermuterTest<int> IntegerPermuterTest; // gotta do this because macros
  TEST_F(IntegerPermuterTest, Three) {
    const int arr[] = {1, 2, 3};
    SetUp(arr, 3, cmp<int>);
    const int correct_permutations[Factorial<3>::value][3] =
      {
        {1, 2, 3},
        {1, 3, 2},
        {3, 1, 2},
        {3, 2, 1},
        {2, 3, 1},
        {2, 1, 3}
      };

    for (int repeat = 0; repeat < 2; repeat++) {
      void *permutation;
      int index = 0;
      for_permutations(p, permutation) {
        ASSERT_TRUE(permutation != nullptr);

        int i = permutation_index(p);
        ASSERT_GE(i, 0);
        EXPECT_EQ(i, index);
        bool equal = arr_eq<int, 3>(correct_permutations[i],
                                    static_cast<const int *>(permutation));
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
      for_permutations(p, permutation)num_permutations++;
      EXPECT_EQ(num_permutations, factorial(strlen(s)));
      reset_permuter(p);
    }
  }

  /**
   * Make sure that 9! permutations happen.
   * If you're thinking this test is over-kill - remember that permuter
   * uses a single bit per element for it's current direction. Without
   * testing more than 8 elements we would only ever test using a single
   * uint8_t. By testing nine elements, we require that the permuter
   * uses more than one uint8_t to keep track of all the directions
   * of the nine different elements.
   */
  TEST_F(StringPermuterTest, NineCount) {
    const char *s = "123456789";
    SetUp(s);
    for (int repeat = 0; repeat < 2; repeat++) {
      int num_permutations = 0;
      void *permutation;
      for_permutations(p, permutation)num_permutations++;
      EXPECT_EQ(num_permutations, factorial(strlen(s)));
      reset_permuter(p);
    }
  }

}

#endif //PERMUTATION_TEST_HPP
