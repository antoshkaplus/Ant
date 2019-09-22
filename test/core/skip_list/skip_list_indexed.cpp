

#include "gtest/gtest.h"

#include "ant/core/skip_list/skip_list_indexed.hpp"

using namespace ant;
using namespace std;

TEST(SkipListIndexed, constructor) {

    SkipListIndexed<int> s_0(0);
    SkipListIndexed<int> s_1(1);
    SkipListIndexed<int> s_2(2);
}

TEST(SkipListIndexed, empty) {
    SkipListIndexed<int> s(1);
    ASSERT_TRUE(s.empty());

    s.InsertAt(s.size(), 90);
    s.RemoveAt(s.size()-1);
    ASSERT_TRUE(s.empty());
}

TEST(SkipListIndexed, ConstIterator) {
    const SkipListIndexed<int> s(80);
    for (auto& p : s) p;
}

class SkipListIndexedTest : public testing::TestWithParam<ant::Count> {
protected:
    std::set<int> ss;
    SkipListIndexed<int> skip_list {1};

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        auto size = GetParam();

        skip_list = SkipListIndexed<int>{size};

        index_distr = uniform_int_distribution<>{0, size-1};
    }

    void CheckEqual() {
        ASSERT_EQ(ss.size(), skip_list.size());
        ASSERT_TRUE(std::equal(ss.begin(), ss.end(), skip_list.begin()));
    }
};

INSTANTIATE_TEST_SUITE_P(InstantiationName,
                         SkipListIndexedTest,
                         testing::Values(1, 10, 100));

TEST_P(SkipListIndexedTest, Count) {

    const auto iterations = GetParam() * GetParam();

    const auto INSERT = 0;
    const auto REMOVE = 1;
    const auto GET = 2;
    discrete_distribution<int> opDistr({0.4, 0.1, 0.5});

    vector<int> vs;
    default_random_engine rng;
    for (auto i = 0; i < iterations; ++i) {
        auto op = opDistr(rng);
        if (vs.size() == 0) {
            op = 0;
        }
        switch (op) {
            case INSERT: { // insert
                uniform_int_distribution<> idxDistr(0, vs.size());
                Index idx = idxDistr(rng);

                vs.insert(vs.begin() + idx, i);
                skip_list.InsertAt(idx, i);
                break;
            }
            case REMOVE: { // remove
                uniform_int_distribution<> idxDistr(0, vs.size()-1);
                Index idx = idxDistr(rng);

                vs.erase(vs.begin() + idx);
                skip_list.RemoveAt(idx);
                break;
            }
            case GET: { // get
                uniform_int_distribution<> idxDistr(0, vs.size()-1);
                Index idx = idxDistr(rng);

                ASSERT_EQ(vs[idx], skip_list[idx]);
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }
    }
    ASSERT_TRUE(std::equal(vs.begin(), vs.end(), skip_list.begin()));
}


