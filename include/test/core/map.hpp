#pragma once

#include "gtest/gtest.h"

#include "ant/core/core.hpp"

using namespace ant;
using namespace std;

template <template <typename, typename> typename Map_, ant::Count kSize_>
struct MapTestCase {
    template <typename Key, typename Value> using Map = Map_<Key, Value>;
    constexpr static Count kSize = kSize_;
};

//USER DEFINED
//template <typename TestCase, typename Key, typename Value>
//typename TestCase::template Map<Key, Value> MakeSet(size_t capacity);

template <typename TestCase>
class MapTest : public testing::Test {
protected:
    template <typename Key, typename Value> using Map = typename TestCase::template Map<Key, Value> ;

    std::map<int, int> controlling_map {};
    Map<int, int> test_map = MakeMap<TestCase, int, int>(TestCase::kSize);

    std::default_random_engine rng;

    uniform_int_distribution<> index_distr;
    uniform_int_distribution<> value_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        test_map = MakeMap<TestCase, int, int>(TestCase::kSize);

        index_distr = uniform_int_distribution<>{0, TestCase::kSize-1};
    }

    void CheckEqual() {
        ASSERT_EQ(controlling_map.size(), test_map.size());
        ASSERT_TRUE(std::equal(controlling_map.begin(), controlling_map.end(), test_map.begin()));
    }
};

TYPED_TEST_SUITE_P(MapTest);

TYPED_TEST_P(MapTest, constructor) {

    auto s_0 = MakeMap<TypeParam, int, int>(0);
    auto s_1 = MakeMap<TypeParam, int, int>(1);
    auto s_2 = MakeMap<TypeParam, int, int>(2);
}

TYPED_TEST_P(MapTest, empty) {
    auto s = MakeMap<TypeParam, int, int>(1);
    ASSERT_TRUE(s.empty());

    s.Insert(1, 1);
    s.Remove(2);
    ASSERT_FALSE(s.empty());

    s.Remove(1);
    ASSERT_TRUE(s.empty());
}

TYPED_TEST_P(MapTest, ConstIterator) {
    const auto s = MakeMap<TypeParam, int, int>(80);
    for (auto& p : s) p;
}

TYPED_TEST_P(MapTest, Iterator) {
    const auto s = MakeMap<TypeParam, int, int>(80);
    for (auto& p : s) p.second = 7;
}

TYPED_TEST_P(MapTest, Count) {
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
                    auto v = this->value_distr(this->rng);

                    this->controlling_map.insert(k, v);
                    this->test_map.Insert(k, v);

                    ASSERT_TRUE(this->test_map.Count(k) == 1);

                    if constexpr (kMicroCheck) this->CheckEqual();
                }
                break;
            }
            case REMOVE: {
                for (auto n = 0, N = this->index_distr(this->rng); n < N; ++n) {
                    auto k = this->index_distr(this->rng);

                    this->controlling_map.erase(k);
                    this->test_map.Remove(k);

                    ASSERT_TRUE(this->test_map.Count(k) == 0);

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

REGISTER_TYPED_TEST_SUITE_P(MapTest,
                            constructor, empty, ConstIterator, Count);

//USER DEFINED
//INSTANTIATE_TYPED_TEST_SUITE_P(SET_TYPE, SetTest, TEST_CASE_TYPES);