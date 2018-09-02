#ifndef LISP_CSET_TEST_HPP
#define LISP_CSET_TEST_HPP

#include <gtest/gtest.h>
#include <cset.h>
#include <ops.h>

namespace {

  template <typename T>
  class SetTest : public testing::Test {
  protected:
    SetTest() : set(nullptr) { }
    using Test::SetUp;
    void SetUp(CMapCmpFn cmp, CleanupFn cleanup) {
      set = new_set(sizeof(T), cmp, cleanup);
      ASSERT_NE(set, nullptr);
    }
    void TestDown() { set_dispose(set); }
    CSet *set;
  };

  typedef SetTest<int> SetTestInt;
  TEST_F(SetTestInt, InsertOne) {
    SetUp(cmp_int, NULL);
    int i = 1;
    set_insert(set, &i);
    const int *eye = static_cast<const int *>(set_lookup(set, &i));
    ASSERT_NE(eye, nullptr);
    ASSERT_EQ(*eye, i);
  }

  TEST_F(SetTestInt, Insert10) {
    SetUp(cmp_int, NULL);

    for (int i = 0; i < 10; i++)
      set_insert(set, &i);

    for (int i = 0; i < 10; i++) {
      const int *eye = static_cast<const int *>(set_lookup(set, &i));
      ASSERT_NE(eye, nullptr);
      ASSERT_EQ(*eye, i);
    }
  }


}


#endif //LISP_CSET_TEST_HPP
