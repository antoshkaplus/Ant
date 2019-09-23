

#include "gtest/gtest.h"

#include "ant/core/skip_list/skip_list_set.hpp"

using namespace ant;
using namespace std;

TEST(Set, constructor) {

    SkipListSet<int> s_0(0);
    SkipListSet<int> s_1(1);
    SkipListSet<int> s_2(2);
}

TEST(SkipListSet, empty) {
    SkipListSet<int> s(1);
    ASSERT_TRUE(s.empty());

    s.Insert(1);
    s.Remove(2);
    ASSERT_FALSE(s.empty());

    s.Remove(1);
    ASSERT_TRUE(s.empty());
}

TEST(SkipListSet, ConstIterator) {
    const SkipListSet<int> s(80);
    for (auto& p : s) p;
}

class SkipListSetTest : public testing::TestWithParam<ant::Count> {
protected:
    std::set<int> ss;
    SkipListSet<int> skip_list {1};

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        auto size = GetParam();

        skip_list = SkipListSet<int>{size};

        index_distr = uniform_int_distribution<>{0, size-1};
    }

    void CheckEqual() {
        ASSERT_EQ(ss.size(), skip_list.size());
        ASSERT_TRUE(std::equal(ss.begin(), ss.end(), skip_list.begin()));
    }
};

INSTANTIATE_TEST_SUITE_P(InstantiationName,
                         SkipListSetTest,
                         testing::Values(1, 10, 100));

TEST_P(SkipListSetTest, Count) {
    constexpr bool kMicroCheck = false;

    const auto iterations = GetParam() * GetParam();

    const auto INSERT = 0;
    const auto REMOVE = 1;

    for (auto i = 0; i < iterations; ++i) {
        auto op = op_distr(rng);
        switch (op) {
            case INSERT: {
                for (auto n = 0, N = index_distr(rng); n < N; ++n) {
                    auto k = index_distr(rng);

                    ss.insert(k);
                    skip_list.Insert(k);

                    ASSERT_TRUE(skip_list.Count(k) == 1);

                    if constexpr (kMicroCheck) CheckEqual();
                }
                break;
            }
            case REMOVE: {
                for (auto n = 0, N = index_distr(rng); n < N; ++n) {
                    auto k = index_distr(rng);

                    ss.erase(k);
                    skip_list.Remove(k);

                    ASSERT_TRUE(skip_list.Count(k) == 0);

                    if constexpr (kMicroCheck) CheckEqual();
                }
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }

        if constexpr (!kMicroCheck) CheckEqual();
    }
}

