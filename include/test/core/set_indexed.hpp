#pragma once

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/core/flat_set.hpp"

using namespace ant;
using namespace std;

template <template <typename> typename SetIndexed_, ant::Count kSize_>
struct SetIndexedTestCase {
    template <typename Value> using SetIndexed = SetIndexed_<Value>;
    constexpr static Count kSize = kSize_;
};

//USER DEFINED
//template <typename TestCase, typename Value>
//typename TestCase::template Set<Value> MakeSetIndexed(size_t capacity);

template <typename TestCase>
class SetIndexedTest : public testing::Test {
protected:
    template <typename Value> using SetIndexed = typename TestCase::template SetIndexed<Value> ;

    FlatSet<int> controlling_set {};
    SetIndexed<int> test_set = MakeSetIndexed<TestCase, int>(TestCase::kSize);

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        test_set = MakeSetIndexed<TestCase, int>(TestCase::kSize);

        index_distr = uniform_int_distribution<>{0, TestCase::kSize-1};
    }

    void CheckEqual() {
        ASSERT_EQ(controlling_set.size(), test_set.size());
        ASSERT_TRUE(std::equal(controlling_set.begin(), controlling_set.end(), test_set.begin()));
        for (auto i = 0; i < controlling_set.size(); ++i) {
            if (controlling_set[i] != test_set[i]) {
                auto j = test_set[i];

            }

            ASSERT_EQ(controlling_set[i], test_set[i]);
            ASSERT_EQ(test_set.Index(controlling_set[i]), std::make_pair(i, true));
        }
    }
};

TYPED_TEST_SUITE_P(SetIndexedTest);

TYPED_TEST_P(SetIndexedTest, simple) {

    auto s = MakeSetIndexed<TypeParam, int>(3);
    s.Insert(0);
    s.Insert(1);
    s.Insert(2);

    for (auto i = 0; i < 3; ++i) ASSERT_EQ(s.Index(i), std::make_pair(i, true));
    ASSERT_EQ(s.size(), 3);

    s.Remove(2);
    ASSERT_EQ(s.size(), 2);

    ASSERT_TRUE(s.Index(0) == std::make_pair(0, true));
    ASSERT_TRUE(s.Index(1) == std::make_pair(1, true));

    ASSERT_FALSE(s.Index(2).second);

    s.Remove(1);

    ASSERT_FALSE(s.Index(1).second);
}

TYPED_TEST_P(SetIndexedTest, InsertRemoveCountIndex) {
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

                    ASSERT_TRUE(this->test_set.Count(k) == 1);

                    if constexpr (kMicroCheck) this->CheckEqual();
                }
                break;
            }
            case REMOVE: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_set.Erase(k);
                    this->test_set.Remove(k);

                    ASSERT_TRUE(this->test_set.Count(k) == 0);

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

REGISTER_TYPED_TEST_SUITE_P(SetIndexedTest,
                            simple, InsertRemoveCountIndex);

//USER DEFINED
//INSTANTIATE_TYPED_TEST_SUITE_P(SET_TYPE, SetTest, TEST_CASE_TYPES);