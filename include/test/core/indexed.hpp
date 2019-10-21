#pragma once

#include "gtest/gtest.h"

#include "ant/core/skip_list/skip_list_indexed.hpp"

using namespace ant;
using namespace std;

template <template <typename> typename Indexed_, ant::Count kSize_>
struct IndexedTestCase {
    template <typename Value> using Indexed = Indexed_<Value>;
    constexpr static Count kSize = kSize_;
};

//USER DEFINED
//template <typename TestCase, typename Value>
//typename TestCase::template Indexed<Value> MakeIndexed(size_t capacity);

template <typename TestCase>
class IndexedTest : public testing::Test {
protected:
    template <typename Value> using Indexed = typename TestCase::template Indexed<Value> ;

    std::vector<int> controlling_container;
    Indexed<int> test_container = MakeIndexed<TestCase, int>(TestCase::kSize);

    std::default_random_engine rng;

    std::uniform_int_distribution<> index_distr;
    std::uniform_int_distribution<> op_distr {0, 1};

    void SetUp() override {
        test_container = MakeIndexed<TestCase, int>(TestCase::kSize);

        index_distr = uniform_int_distribution<>{0, TestCase::kSize-1};
    }

    void CheckEqual() {
        ASSERT_EQ(controlling_container.size(), test_container.size());
        ASSERT_TRUE(std::equal(controlling_container.begin(), controlling_container.end(), test_container.begin()));

        for (auto i = 0; i < controlling_container.size(); ++i) {
            ASSERT_EQ(controlling_container[i], test_container[i]);
        }
    }
};

TYPED_TEST_SUITE_P(IndexedTest);

TYPED_TEST_P(IndexedTest, constructor) {
    auto s_0 = MakeIndexed<TypeParam, int>(0);
    auto s_1 = MakeIndexed<TypeParam, int>(1);
    auto s_2 = MakeIndexed<TypeParam, int>(2);
}

TYPED_TEST_P(IndexedTest, empty) {
    auto s = MakeIndexed<TypeParam, int>(1);
    ASSERT_TRUE(s.empty());

    s.InsertAt(s.size(), 90);
    s.RemoveAt(s.size()-1);
    ASSERT_TRUE(s.empty());
}

TYPED_TEST_P(IndexedTest, ConstIterator) {
    const auto s = MakeIndexed<TypeParam, int>(80);
    for (auto& p : s) p;
}

TYPED_TEST_P(IndexedTest, Iterator) {
    auto s = MakeIndexed<TypeParam, int>(80);
    for (auto& p : s) p = 90;
}

TYPED_TEST_P(IndexedTest, Count) {

    const auto iterations = TypeParam::kSize * TypeParam::kSize;

    const auto INSERT = 0;
    const auto REMOVE = 1;
    const auto GET = 2;
    discrete_distribution<int> opDistr({0.4, 0.1, 0.5});

    default_random_engine rng;
    for (auto i = 0; i < iterations; ++i) {
        auto op = opDistr(rng);
        if (this->controlling_container.size() == 0) {
            op = 0;
        }
        switch (op) {
            case INSERT: { // insert
                uniform_int_distribution<> idxDistr(0, this->controlling_container.size());
                Index idx = idxDistr(rng);

                this->controlling_container.insert(this->controlling_container.begin() + idx, i);
                this->test_container.InsertAt(idx, i);
                break;
            }
            case REMOVE: { // remove
                uniform_int_distribution<> idxDistr(0, this->controlling_container.size()-1);
                Index idx = idxDistr(rng);

                this->controlling_container.erase(this->controlling_container.begin() + idx);
                this->test_container.RemoveAt(idx);
                break;
            }
            case GET: { // get
                uniform_int_distribution<> idxDistr(0, this->controlling_container.size()-1);
                Index idx = idxDistr(rng);

                ASSERT_EQ(this->test_container[idx], this->controlling_container[idx]);
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }
    }
    this->CheckEqual();
}

REGISTER_TYPED_TEST_SUITE_P(IndexedTest,
                            constructor, empty, ConstIterator, Iterator, Count);

//USER DEFINED
//INSTANTIATE_TYPED_TEST_SUITE_P(SET_TYPE, SetTest, TEST_CASE_TYPES);

