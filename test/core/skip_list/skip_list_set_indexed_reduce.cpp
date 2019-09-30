

#include "gtest/gtest.h"

#include "ant/core/skip_list/skip_list_set_indexed_reduce.hpp"

using namespace ant;
using namespace std;

int sum(int i_1, int i_2) { return i_1 + i_2; }

// SET

template <typename Value>
using SkipListSetIndexedReduceSUM = SkipListSetIndexedReduce<int, decltype(&sum)>;

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Set<Value>,
                SkipListSetIndexedReduceSUM<Value>>::value>::type* = nullptr>
auto MakeSet(size_t capacity) {
    return SkipListSetIndexedReduceSUM<Value>(capacity, sum);
}

#include "test/core/set.hpp"

using SkipListSetIndexedReduceSUM_SetTestTypes = ::testing::Types<
        SetTestCase<SkipListSetIndexedReduceSUM, 1>,
        SetTestCase<SkipListSetIndexedReduceSUM, 10>,
        SetTestCase<SkipListSetIndexedReduceSUM, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(SkipListSetIndexedReduceSUM, SetTest, SkipListSetIndexedReduceSUM_SetTestTypes);


// SET INDEXED

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template SetIndexed<Value>,
                SkipListSetIndexedReduceSUM<Value>>::value>::type* = nullptr>
auto MakeSetIndexed(size_t capacity) {
    return SkipListSetIndexedReduceSUM<Value>(capacity, sum);
}

#include "test/core/set_indexed.hpp"

using SkipListSetIndexedReduceSUM_SetIndexedTestTypes = ::testing::Types<
        SetIndexedTestCase<SkipListSetIndexedReduceSUM, 1>,
        SetIndexedTestCase<SkipListSetIndexedReduceSUM, 10>,
        SetIndexedTestCase<SkipListSetIndexedReduceSUM, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(SkipListSetIndexedReduceSUM, SetIndexedTest, SkipListSetIndexedReduceSUM_SetIndexedTestTypes);
