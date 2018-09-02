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


}


#endif //LISP_CSET_TEST_HPP
