
#include "gtest/gtest.h"

#include "ant/core/skip_list/base/node.hpp"
#include "ant/core/skip_list/base/reduce.hpp"

using namespace ant;
using namespace ant::core::skip_list::base;
using namespace std;

static int sum(int i_1, int i_2) { return i_1 + i_2; }


TEST(SkipList_Base_Reduce, PushBack) {
    constexpr bool VERBOSE = false;

    using R = Reduce<int>;
    using Node = R::Node;

    auto head = std::make_shared<Node>(3);

    std::vector<int> heights = {1, 2, 1, 2, 3, 1, 2};
    std::vector<int> values =  {1, 1, 1, 1, 1, 1, 1};

    if constexpr (VERBOSE) {
        for (auto [h, v] : ZipRange(heights, values)) {
            R::PushBack(head, v, h, sum);
            R::Println(std::cout, head);
        }
        R::Println(std::cout, head);
    }
}

TEST(SkipList_Base_Reduce, ReduceAfter) {
    constexpr bool VERBOSE = false;

    using R = Reduce<int>;
    using Node = R::Node;

    auto head = std::make_shared<Node>(3);

    std::vector<int> heights = {1, 2, 1, 2, 3, 1, 2, 3, 1};
    std::vector<int> values =  {0, 1, 3, 4, 5, 6, 7, 8, 9};

    std::vector<std::shared_ptr<Node>> nodes;

    for (auto [h, v] : ZipRange(heights, values)) {
        nodes.push_back( R::PushBack(head, v, h, sum) );
    }

    if constexpr (VERBOSE) {
        R::Println(std::cout, head);

        R::Println(std::cout, R::ReduceAfter(head, 2, 10, sum));
        R::Println(std::cout, R::ReduceAfter(head, 1, 5, sum));
        R::Println(std::cout, R::ReduceAfter(head, 0, 7, sum));
        R::Println(std::cout, R::ReduceAfter(head, 2, 2, sum));
        R::Println(std::cout, R::ReduceAfter(head, 1, 2, sum));
    }
}

TEST(SkipList_Base_Reduce, InsertBetween_1) {
    // if I push back the result should be the same if as I insert in between.

    // funny stuff

    //    std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i,
//            Count newHeight, Value&& newVal, Op& op
}