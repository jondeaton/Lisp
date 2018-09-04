#ifndef LISP_CSET_TEST_HPP
#define LISP_CSET_TEST_HPP

#include <gtest/gtest.h>
#include <random>
#include <set>
//#include <vector>
#include <limits>
#include <algorithm>

#include <cset.h>
#include <ops.h>

namespace {

  template <typename T, CmpFn Compare, CleanupFn Cleanup, int Seed=0>
  class SetTest : public testing::Test {
  protected:
    typedef T type;
    SetTest() : set(nullptr),
                mt(Seed),
                dist(std::numeric_limits<T>::min(),
                     std::numeric_limits<T>::max()) { }

    using Test::SetUp;
    void SetUp() {
      set = new_set(sizeof(T), Compare, Cleanup);
      ASSERT_NE(set, nullptr);
    }
    T random() { return dist(mt); }
    void TestDown() { set_dispose(set); }

    CSet *set;
    std::mt19937 mt;
    std::uniform_int_distribution<T> dist;
  };

  typedef SetTest<int, cmp_int, nullptr> SetTestInt;

  TEST_F(SetTestInt, StartsEmpty) {
    SetUp();
    EXPECT_EQ(set_size(set), 0);
  }

  TEST_F(SetTestInt, InsertOne) {
    SetUp();
    int i = 1;
    set_insert(set, &i);
    auto aye = static_cast<const int *>(set_lookup(set, &i));
    ASSERT_NE(aye, nullptr);
    EXPECT_EQ(*aye, i);
  }

  TEST_F(SetTestInt, InsertDuplicate) {
    SetUp();
    int i = 7;
    int j = 7;
    set_insert(set, &i);
    set_insert(set, &j);
    EXPECT_EQ(set_size(set), 1);
  }

  TEST_F(SetTestInt, Insert10) {
    SetUp();

    for (int i = 0; i < 10; i++)
      set_insert(set, &i);

    for (int i = 0; i < 10; i++) {
      auto aye = static_cast<const int *>(set_lookup(set, &i));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, i);
    }
  }

  TEST_F(SetTestInt, RotateRight) {
    SetUp();

    std::vector<type> elements = { 1, 2, 3 };
    for (auto el : elements)
      set_insert(set, &el);

    EXPECT_EQ(set_size(set), elements.size());

    for (auto el : elements) {
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, RotateLeft) {
    SetUp();

    std::vector<type> elements = { 3, 2, 1 };
    for (auto el : elements)
      set_insert(set, &el);

    EXPECT_EQ(set_size(set), elements.size());

    for (auto el : elements) {
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, RotateRightSub) {
    SetUp();

    std::vector<type> elements = { 3, 1, 2 };
    for (auto el : elements)
      set_insert(set, &el);

    EXPECT_EQ(set_size(set), elements.size());

    for (auto el : elements) {
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, RotateLeftSub) {
    SetUp();

    std::vector<type> elements = { 1, 3, 2 };
    for (auto el : elements)
      set_insert(set, &el);

    EXPECT_EQ(set_size(set), elements.size());

    for (auto el : elements) {
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  // make sure you can insert a whole bunch of numbers
  TEST_F(SetTestInt, Insert10k) {
    SetUp();
    int N = 10 * (1 << 10);
    for (int i = 0; i < N; i++)
      set_insert(set, &i);

    for (int i = 0; i < N; i++) {
      auto aye = static_cast<const int *>(set_lookup(set, &i));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, i);
    }
  }

  TEST_F(SetTestInt, InsertDescending) {
    SetUp();

    for (int i = 9; i >= 0; i--)
      set_insert(set, &i);

    for (int i = 0; i < 10; i++) {
      auto aye = static_cast<const int *>(set_lookup(set, &i));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, i);
    }
  }

  TEST_F(SetTestInt, Size) {
    SetUp();

    int N = 100;

    for (int i = 0; i < N; i++)
      set_insert(set, &i);

    EXPECT_EQ(set_size(set), N);

    set_clear(set);
    EXPECT_EQ(set_size(set), 0);
  }

  TEST_F(SetTestInt, Delete) {
    SetUp();

    int i = 3;
    set_insert(set, &i);
    EXPECT_NE(set_lookup(set, &i), nullptr);
    set_remove(set, &i);
    EXPECT_EQ(set_lookup(set, &i), nullptr);
  }

  TEST_F(SetTestInt, DeleteAbsent) {
    SetUp();

    int i = 123;
    int j = 345;
    set_insert(set, &i);
    set_remove(set, &j);
    EXPECT_NE(set_lookup(set, &i), nullptr);
    EXPECT_EQ(set_size(set), 1);
  }

  TEST_F(SetTestInt, DeleteSingle) {
    SetUp();
    int N = 10;
    for (int i = 0; i < N; i++)
      set_insert(set, &i);

    int d = 100;
    set_insert(set, &d);
    ASSERT_NE(set_lookup(set, &d), nullptr);
    set_remove(set, &d);
    ASSERT_EQ(set_lookup(set, &d), nullptr);

    EXPECT_EQ(set_size(set), N);

    // make sure all the others are in there
    for (int i = 0; i < N; i++) {
      auto aye = static_cast<const int *>(set_lookup(set, &i));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, i);
    }
  }

  TEST_F(SetTestInt, DeleteRightRotate) {
    SetUp();

    std::vector<type> elements = { 20, 10, 30, 15 };
    for (auto el : elements)
      set_insert(set, &el);

    int d = 30;
    set_remove(set, &d);

    EXPECT_EQ(set_lookup(set, &d), nullptr);
    EXPECT_EQ(set_size(set), elements.size() - 1);

    for (auto el : elements) {
      if (el == d) continue;
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, DeleteLeftRotate) {
    SetUp();

    std::vector<type> elements = { 20, 10, 30, 25 };
    for (auto el : elements)
      set_insert(set, &el);

    int d = 10;
    set_remove(set, &d);

    EXPECT_EQ(set_lookup(set, &d), nullptr);
    EXPECT_EQ(set_size(set), elements.size() - 1);

    for (auto el : elements) {
      if (el == d) continue;
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, DeleteRecursiveRotate) {
    SetUp();

    std::vector<type> elements = { 100, 20, 120, 10, 110, 30, 130, 15,
                                   115, 125, 140, 135 };
    for (auto el : elements)
      set_insert(set, &el);

    int d = 30;
    set_remove(set, &d);

    EXPECT_EQ(set_lookup(set, &d), nullptr);
    EXPECT_EQ(set_size(set), elements.size() - 1);

    for (auto el : elements) {
      if (el == d) continue;
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, DeleteNonLeaf) {
    SetUp();

    std::vector<type> elements = { 100, 20, 120, 10, 110, 30, 130, 15,
                                   115, 125, 140, 135 };
    for (auto el : elements)
      set_insert(set, &el);

    int d = 20;
    set_remove(set, &d);

    EXPECT_EQ(set_lookup(set, &d), nullptr);
    EXPECT_EQ(set_size(set), elements.size() - 1);

    for (auto el : elements) {
      if (el == d) continue;
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      EXPECT_EQ(*aye, el);
    }
  }

  TEST_F(SetTestInt, DeleteMultiple) {
    SetUp();

    std::set<type> all = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::set<type> deleted = { 3, 5, 7, 9 };

    std::set<type> remaining;
    std::set_difference(all.begin(), all.end(),
                        deleted.begin(), deleted.end(),
                        std::inserter(remaining, remaining.end()));

    for (auto el : all)
      set_insert(set, &el);

    for (auto el : deleted) {
      set_remove(set, &el);

      for (auto e : remaining) {
        auto aye = static_cast<const int *>(set_lookup(set, &e));
        ASSERT_NE(aye, nullptr);
        EXPECT_EQ(*aye, e);
      }
    }
  }

  TEST_F(SetTestInt, RankOne) {
    SetUp();
    int i = 123;
    set_insert(set, &i);
    EXPECT_EQ(set_rank(set, &i), 0);
  }

  TEST_F(SetTestInt, RankNoRotate) {
    SetUp();
    std::vector<type> elements = { 15, 10, 20, 30 };
    for (auto el : elements)
      set_insert(set, &el);

    EXPECT_EQ(set_rank(set, &elements[0]), 1);
    EXPECT_EQ(set_rank(set, &elements[1]), 0);
    EXPECT_EQ(set_rank(set, &elements[2]), 2);
    EXPECT_EQ(set_rank(set, &elements[3]), 3);
  }

  TEST_F(SetTestInt, RankRotateRight) {
    SetUp();
    std::vector<type> elements = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (auto el : elements)
      set_insert(set, &el);

    for (auto el : elements)
      EXPECT_EQ(set_rank(set, &el), el);
  }

  TEST_F(SetTestInt, RankRotateLeft) {
    SetUp();
    std::vector<type> elements = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    for (auto el : elements)
      set_insert(set, &el);

    for (auto el : elements)
      EXPECT_EQ(set_rank(set, &el), el);
  }

  TEST_F(SetTestInt, RankAbsent) {
    SetUp();
    std::vector<type> elements = { 20, 10, 15, 30 };
    for (auto el : elements)
      set_insert(set, &el);

    int e = 72;
    EXPECT_EQ(set_rank(set, &e), CSET_ERROR);
  }

  TEST_F(SetTestInt, RankMany) {
    SetUp();
    int N = 1 << 14;

    std::set<type> elements;
    for (int i = 0; i < N; i++) {
      int random_element = random();
      set_insert(set, &random_element);
      elements.insert(random_element);
    }

    std::vector<type> distinct_els;
    for (auto el : elements)
      distinct_els.push_back(el);
    std::sort(distinct_els.begin(), distinct_els.end());

    for (int i = 0; i < (int) distinct_els.size(); i++)
      EXPECT_EQ(set_rank(set, &distinct_els[i]), i);
  }

  TEST_F(SetTestInt, Clear) {
    SetUp();

    for (int i = 0; i < 100; i++)
      set_insert(set, &i);

    set_clear(set);
    EXPECT_EQ(set_size(set), 0);

    // make sure they're all gone
    for (int i = 0; i < 100; i++)
      EXPECT_EQ(set_lookup(set, &i), nullptr);
  }

  TEST_F(SetTestInt, BigInsertDelete) {
    SetUp();

    int N = 1 << 13;

    std::set<type> reference_set;   // all the elements that are inserted
    std::set<type> deletion_set;    // those which become deleted

    for (int i = 0; i < N; i++) {
      int random_element = random();
      set_insert(set, &random_element);

      reference_set.insert(random_element); // keep track
      if (i % 2 == 0)
        deletion_set.insert(random_element);  // delete half of them
    }

    EXPECT_EQ(reference_set.size(), set_size(set));

    // make sure they're all in there
    for (auto el : reference_set)
      ASSERT_NE(reference_set.find(el), reference_set.end());

    // delete half of them
    for (auto el : deletion_set)
      set_remove(set, &el);

    // make sure they're all deleted
    for (auto el : deletion_set)
      ASSERT_EQ(set_lookup(set, &el), nullptr);

    // Make sure that the ones that didn't get deleted are still there
    std::set<type> remaining;
    std::set_difference(reference_set.begin(), reference_set.end(),
                        deletion_set.begin(), deletion_set.end(),
                        std::inserter(remaining, remaining.end()));

    EXPECT_EQ(remaining.size(), set_size(set));

    for (auto el : remaining) {
      auto aye = static_cast<const int *>(set_lookup(set, &el));
      ASSERT_NE(aye, nullptr);
      ASSERT_EQ(*aye, el);
    }
  }
}

#endif //LISP_CSET_TEST_HPP
