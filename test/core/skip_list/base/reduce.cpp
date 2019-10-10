
#include "gtest/gtest.h"

#include "ant/core/skip_list/base/node.hpp"
#include "ant/core/skip_list/base/reduce.hpp"

using namespace ant;
using namespace ant::core::skip_list::base;
using namespace std;

TEST(SkipList_Base_Reduce, ReduceInsertBetween) {

    using Node = NodeIndexedReduce<int>;

    auto head = std::make_shared<Node>(3);
    std::vector<int> heights = {1, 2, 1, 2, 3, 1, 2};
    auto value = 0;
    auto cur = head;
    for (auto h : heights) {
        head
    }




    SkipListIndexed<int> s_1(1);
    SkipListIndexed<int> s_2(2);
}