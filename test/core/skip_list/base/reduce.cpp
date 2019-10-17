
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

static void TestInsert(const int seed, const Count size, const Count max_height, Index insert_index) {
    start:

    using R = Reduce<int>;
    using Node = R::Node;

    std::default_random_engine rng(seed);
    std::uniform_int_distribution height_distr(1, max_height);

    std::vector<int> heights;
    std::generate_n(std::back_inserter(heights), size, [&]() { return height_distr(rng); });

    std::shared_ptr<Node> head_PushBack = std::make_shared<Node>(max_height);
    std::shared_ptr<Node> head_Insert = std::make_shared<Node>(max_height);

    std::vector<int> values(size);
    std::iota(values.begin(), values.end(), 1);

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

static void TestInsert(const int seed, const Count size, const Count max_height) {
    for (auto i = 0; i < size; ++i) TestInsert(seed, size, max_height, i);
}

TEST(SkipList_Base_Reduce, Insert_Small) {
    std::default_random_engine rng;
    std::uniform_int_distribution seed_distr;

    for (auto values : {std::vector<int>{1},
                        std::vector<int>{1, 2},
                        std::vector<int>{1, 2, 3},
                        std::vector<int>{1, 2, 3, 4},
                        std::vector<int>{1, 2, 3, 4, 5}}) {
        for (auto max_height : {1, 2, 3, 4, 5}) {
            for (auto samples = 0; samples < max_height * 10; ++samples) {
                TestInsert(seed_distr(rng), values.size(), max_height);
            }
        }
    }
}

TEST(SkipList_Base_Reduce, Insert_Big) {
    std::default_random_engine rng;
    std::uniform_int_distribution seed_distr;

    for (auto values_count : {10, 100}) {
        std::vector<int> values(values_count);
        std::iota(values.begin(), values.end(), 1);

        for (auto max_height : {1, 2, 4, 8, 16}) {
            for (auto samples = 0; samples < 3*max_height; ++samples) {
                TestInsert(seed_distr(rng), values.size(), max_height);
            }
        }
    }
}


static void TestRemove(const int seed, const Count size, const Count max_height, Index remove_index) {
    start:

    using R = Reduce<int>;
    using Node = R::Node;

    std::default_random_engine rng(seed);
    std::uniform_int_distribution height_distr(1, max_height);

    std::vector<int> heights;
    std::generate_n(std::back_inserter(heights), size, [&]() { return height_distr(rng); });

    std::shared_ptr<Node> head_PushBack = std::make_shared<Node>(max_height);
    std::shared_ptr<Node> head_Remove = std::make_shared<Node>(max_height);

    std::vector<int> values(size);
    std::iota(values.begin(), values.end(), 1);

    for (auto[i, h, v] : iZipRange(heights, values)) {
        R::PushBack(head_Remove, v, h, sum);
        if (i != remove_index) R::PushBack(head_PushBack, v, h, sum);
    }
    R::Remove(head_Remove, values[remove_index], sum);

    auto range_PushBack = ant::core::skip_list::base::Range(head_PushBack);
    auto range_Remove = ant::core::skip_list::base::Range(head_Remove);

    if (!std::equal(
            range_Remove.begin(), range_Remove.end(),
            range_PushBack.begin(), range_PushBack.end())) {
        std::cout << "remove: height " << max_height << " index " << remove_index << " value "
                  << values[remove_index] << std::endl;
        std::cout << "expect:" << std::endl;
        R::Println(std::cout, head_PushBack);
        std::cout << "test:" << std::endl;
        R::Println(std::cout, head_Remove);

        ASSERT_TRUE(false);

        goto start;
    }
}

static void TestRemove(const int seed, const Count size, const Count max_height) {
    for (auto i = 0; i < size; ++i) TestRemove(seed, size, max_height, i);
}

TEST(SkipList_Base_Reduce, Remove_Small) {
    std::default_random_engine rng;
    std::uniform_int_distribution seed_distr;

    for (auto values : {std::vector<int>{1},
                        std::vector<int>{1, 2},
                        std::vector<int>{1, 2, 3},
                        std::vector<int>{1, 2, 3, 4},
                        std::vector<int>{1, 2, 3, 4, 5}}) {
        for (auto max_height : {1, 2, 3, 4, 5}) {
            for (auto samples = 0; samples < max_height * 10; ++samples) {
                TestRemove(seed_distr(rng), values.size(), max_height);
            }
        }
    }
}

TEST(SkipList_Base_Reduce, Remove_Big) {
    std::default_random_engine rng;
    std::uniform_int_distribution seed_distr;

    for (auto values_count : {10, 100}) {
        std::vector<int> values(values_count);
        std::iota(values.begin(), values.end(), 1);

        for (auto max_height : {1, 2, 4, 8, 16}) {
            for (auto samples = 0; samples < 3*max_height; ++samples) {
                TestRemove(seed_distr(rng), values.size(), max_height);
            }
        }
    }
}
