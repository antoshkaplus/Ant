
#include "gtest/gtest.h"

#include "ant/core/count_index.hpp"

using namespace ant;
using namespace std;

TEST(CountIndex, allin) {

    CountIndex<> counter;
    ASSERT_EQ(counter[0], 0);
    ASSERT_EQ(counter[10], 0);
    ASSERT_EQ(counter[100], 0);
    ASSERT_TRUE(counter.empty());

    counter.Increase(10);
    ASSERT_EQ(counter[10], 1);
    ASSERT_FALSE(counter.empty());

    counter.Decrease(10);
    ASSERT_EQ(counter[10], 0);
    ASSERT_TRUE(counter.empty());

    counter.Increase(100);
    counter.Increase(100);
    ASSERT_EQ(counter[100], 2);
    ASSERT_EQ(counter.size(), 1);
}