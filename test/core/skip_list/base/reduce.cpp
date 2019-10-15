
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

TEST(SkipList_Base_Reduce, Insert) {
    using R = Reduce<int>;
    using Node = R::Node;

    for (auto values : {std::vector<int>{1},
                        std::vector<int>{1, 2},
                        std::vector<int>{1, 2, 3},
                        std::vector<int>{1, 2, 3, 4},
                        std::vector<int>{1, 2, 3, 4, 5}}) {

        std::vector<int> indexes(values.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        for (auto max_height : {1, 2, 3, 4, 5}) {
            for (auto insert_index : indexes) {
                start:

                std::default_random_engine rng;
                std::uniform_int_distribution height_distr(1, max_height);

                std::vector<int> heights;
                std::generate_n(std::back_inserter(heights), values.size(), [&]() { return height_distr(rng); });

                std::shared_ptr<Node> head_PushBack = std::make_shared<Node>(max_height);
                std::shared_ptr<Node> head_Insert = std::make_shared<Node>(max_height);

                for (auto[i, h, v] : iZipRange(heights, values)) {
                    if (i != insert_index) R::PushBack(head_Insert, v, h, sum);
                    R::PushBack(head_PushBack, v, h, sum);
                }
                R::Insert(head_Insert, values[insert_index], heights[insert_index], sum);

                auto range_PushBack = ant::core::skip_list::base::Range(head_PushBack);
                auto range_Insert = ant::core::skip_list::base::Range(head_Insert);

                if (!std::equal(
                        range_Insert.begin(), range_Insert.end(),
                        range_PushBack.begin(), range_PushBack.end())) {
                    std::cout << "insertion: height " << max_height << " index " << insert_index << " value "
                              << values[insert_index] << std::endl;
                    std::cout << "expect:" << std::endl;
                    R::Println(std::cout, head_PushBack);
                    std::cout << "test:" << std::endl;
                    R::Println(std::cout, head_Insert);

                    ASSERT_TRUE(false);

                    goto start;
                }
            }
        }
    }
}

TEST(SkipList_Base_Reduce, Insert_Big) {
    using R = Reduce<int>;
    using Node = R::Node;

    for (auto values_count : {10, 100, 1000}) {
        std::vector<int> values(values_count);
        std::iota(values.begin(), values.end(), 1);

        std::vector<int> indexes(values.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        for (auto max_height : {1, 2, 4, 8, 16}) {
            for (auto insert_index : indexes) {
                start:

                std::default_random_engine rng;
                std::uniform_int_distribution height_distr(1, max_height);

                std::vector<int> heights;
                std::generate_n(std::back_inserter(heights), values.size(), [&]() { return height_distr(rng); });

                std::shared_ptr<Node> head_PushBack = std::make_shared<Node>(max_height);
                std::shared_ptr<Node> head_Insert = std::make_shared<Node>(max_height);

                for (auto[i, h, v] : iZipRange(heights, values)) {
                    if (i != insert_index) R::PushBack(head_Insert, v, h, sum);
                    R::PushBack(head_PushBack, v, h, sum);
                }
                R::Insert(head_Insert, values[insert_index], heights[insert_index], sum);

                auto range_PushBack = ant::core::skip_list::base::Range(head_PushBack);
                auto range_Insert = ant::core::skip_list::base::Range(head_Insert);

                if (!std::equal(
                        range_Insert.begin(), range_Insert.end(),
                        range_PushBack.begin(), range_PushBack.end())) {
                    std::cout << "insertion: height " << max_height << " index " << insert_index << " value "
                              << values[insert_index] << std::endl;
                    std::cout << "expect:" << std::endl;
                    R::Println(std::cout, head_PushBack);
                    std::cout << "test:" << std::endl;
                    R::Println(std::cout, head_Insert);

                    //ASSERT_TRUE(false);

                    goto start;
                }
            }
        }
    }
}