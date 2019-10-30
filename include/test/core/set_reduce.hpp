#pragma once

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/core/flat_set.hpp"

using namespace ant;
using namespace std;

template <template <typename> typename SetReduce_, ant::Count kSize_>
struct SetReduceTestCase {
    template <typename Value> using SetReduce = SetReduce_<Value>;
    constexpr static Count kSize = kSize_;
};

//USER DEFINED
//template <typename TestCase, typename Value>
//typename TestCase::template Set<Value> MakeSetReduce(size_t capacity);

template <typename TestCase>
class SetReduceTest : public testing::Test {
protected:
    template <typename Value> using SetReduce = typename TestCase::template SetReduce<Value> ;

    FlatSet<int> controlling_set {};
    SetIndexed<int> test_set = MakeSetReduce<TestCase, int>(TestCase::kSize);

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    int ReduceControllingSet(Index pos, Count count) {
        return std::accumulate(controlling_set.begin() + pos, controlling_set.begin() + pos + count, 0);
    }

    void SetUp() override {
        test_set = MakeSetReduce<TestCase, int>(TestCase::kSize);

        index_distr = uniform_int_distribution<>{0, TestCase::kSize-1};
    }

    void CheckEqual() {
        ASSERT_EQ(controlling_set.size(), test_set.size());
        ASSERT_TRUE(std::equal(controlling_set.begin(), controlling_set.end(), test_set.begin()));
        for (auto i = 0; i < controlling_set.size(); ++i) {
            for (auto k = 1; k <= controlling_set.size(); ++k) {
                ASSERT_EQ(ReduceControllingSet(i, k), test.Reduce(i, k));
            }
        }
    }
};

TYPED_TEST_SUITE_P(SetReduceTest);

TYPED_TEST_P(SetReduceTest, InsertRemove) {
    constexpr bool kMicroCheck = true;

    const auto iterations = TypeParam::kSize * TypeParam::kSize;

    const auto INSERT = 0;
    const auto REMOVE = 1;

    for (auto i = 0; i < iterations; ++i) {
        auto op = this->op_distr(this->rng);
        switch (op) {
            case INSERT: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_set.Insert(k);
                    this->test_set.Insert(k);

                    if constexpr (kMicroCheck) this->CheckEqual();
                }
                break;
            }
            case REMOVE: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_set.Erase(k);
                    this->test_set.Remove(k);

                    if constexpr (kMicroCheck) this->CheckEqual();
                }
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }

        if constexpr (!kMicroCheck) this->CheckEqual();
    }
}

REGISTER_TYPED_TEST_SUITE_P(SetReduceTest, InsertRemove);

//USER DEFINED
//INSTANTIATE_TYPED_TEST_SUITE_P(SET_TYPE, SetTest, TEST_CASE_TYPES);