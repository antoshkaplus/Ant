#pragma once

#include "gtest/gtest.h"

#include "ant/core/core.hpp"

using namespace ant;
using namespace std;

template <template <typename> typename Set_, ant::Count kSize_>
struct SetTestCase {
    template <typename Value> using Set = Set_<Value>;
    constexpr static Count kSize = kSize_;
};

//USER DEFINED
//template <typename TestCase, typename Value>
//typename TestCase::template Set<Value> MakeSet(size_t capacity);

template <typename TestCase>
class SetTest : public testing::Test {
protected:
    template <typename Value> using Set = typename TestCase::template Set<Value> ;

    std::set<int> controlling_set {};
    Set<int> test_set = MakeSet<TestCase, int>(TestCase::kSize);

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        test_set = MakeSet<TestCase, int>(TestCase::kSize);

        index_distr = uniform_int_distribution<>{0, TestCase::kSize-1};
    }

    void CheckEqual() {
        ASSERT_EQ(controlling_set.size(), test_set.size());
        ASSERT_TRUE(std::equal(controlling_set.begin(), controlling_set.end(), test_set.begin()));
    }
};

TYPED_TEST_SUITE_P(SetTest);

TYPED_TEST_P(SetTest, constructor) {

    auto s_0 = MakeSet<TypeParam, int>(0);
    auto s_1 = MakeSet<TypeParam, int>(1);
    auto s_2 = MakeSet<TypeParam, int>(2);
}

TYPED_TEST_P(SetTest, empty) {
    auto s = MakeSet<TypeParam, int>(1);
    ASSERT_TRUE(s.empty());

    s.Insert(1);
    s.Remove(2);
    ASSERT_FALSE(s.empty());

    s.Remove(1);
    ASSERT_TRUE(s.empty());
}

TYPED_TEST_P(SetTest, ConstIterator) {
    const auto s = MakeSet<TypeParam, int>(80);
    for (auto& p : s) p;
}

TYPED_TEST_P(SetTest, Count) {
    constexpr bool kMicroCheck = false;

    const auto iterations = TypeParam::kSize * TypeParam::kSize;

    const auto INSERT = 0;
    const auto REMOVE = 1;

    for (auto i = 0; i < iterations; ++i) {
        auto op = this->op_distr(this->rng);
        switch (op) {
            case INSERT: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_set.insert(k);
                    this->test_set.Insert(k);

                    ASSERT_TRUE(this->test_set.Count(k) == 1);

                    if constexpr (kMicroCheck) this->CheckEqual();
                }
                break;
            }
            case REMOVE: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_set.erase(k);
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

REGISTER_TYPED_TEST_SUITE_P(SetTest,
        constructor, empty, ConstIterator, Count);

//USER DEFINED
//INSTANTIATE_TYPED_TEST_SUITE_P(SET_TYPE, SetTest, TEST_CASE_TYPES);