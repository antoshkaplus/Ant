
#include "gtest/gtest.h"

#include "ant/core/tree/avl/avl_set.hpp"
#include "ant/core/tree/avl/avl_map.hpp"
#include "ant/core/tree/avl/avl_indexed.hpp"
#include "ant/core/tree/avl/avl_set_indexed.hpp"
#include "ant/core/tree/avl/avl_set_reduce.hpp"

using namespace ant;
using namespace ant::core::tree::avl;
using namespace std;

// SET

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Set<Value>,
                AVL_Set<Value>>::value>::type* = nullptr>
auto MakeSet(size_t capacity) {
    return AVL_Set<int>{};
}

#include "test/core/set.hpp"

using AVL_SetTestTypes = ::testing::Types<
        SetTestCase<AVL_Set, 1>,
        SetTestCase<AVL_Set, 10>,
        SetTestCase<AVL_Set, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AVL_Set, SetTest, AVL_SetTestTypes);

// MAP

template <typename TestCase, typename Key, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Map<Key, Value>,
                AVL_Map<Key, Value>>::value>::type* = nullptr>
auto MakeMap(size_t capacity) {
    return AVL_Map<int, int>{};
}

#include "test/core/map.hpp"

using AVL_MapTestTypes = ::testing::Types<
        MapTestCase<AVL_Map, 1>,
        MapTestCase<AVL_Map, 10>,
        MapTestCase<AVL_Map, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AVL_Map, MapTest, AVL_MapTestTypes);

// INDEXED

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Indexed<Value>,
                AVL_Indexed<Value>>::value>::type* = nullptr>
auto MakeIndexed(size_t capacity) {
    return AVL_Indexed<int>();
}

#include "test/core/indexed.hpp"

using AVL_IndexedTestTypes = ::testing::Types<
        IndexedTestCase<AVL_Indexed, 1>,
        IndexedTestCase<AVL_Indexed, 10>,
        IndexedTestCase<AVL_Indexed, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AVL_Indexed, IndexedTest, AVL_IndexedTestTypes);

// SET_INDEXED

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template SetIndexed<Value>,
                AVL_SetIndexed<Value>>::value>::type* = nullptr>
auto MakeSetIndexed(size_t capacity) {
    return AVL_SetIndexed<int>();
}

#include "test/core/set_indexed.hpp"

using AVL_SetIndexedTestTypes = ::testing::Types<
        SetIndexedTestCase<AVL_SetIndexed, 1>,
        SetIndexedTestCase<AVL_SetIndexed, 10>,
        SetIndexedTestCase<AVL_SetIndexed, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AVL_SetIndexed, SetIndexedTest, AVL_SetIndexedTestTypes);

// SET_REDUCE

static int sum(int i_1, int i_2) { return i_1 + i_2; }

template <typename Value>
using AVL_SetReduceSUM = AVL_SetReduce<int, decltype(&sum)>;

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template SetReduce<Value>,
                AVL_SetReduceSUM<Value>>::value>::type* = nullptr>
auto MakeSetReduce(size_t capacity) {
    return AVL_SetReduceSUM<int>(sum);
}

#include "test/core/set_reduce.hpp"

using AVL_SetReduceTestTypes = ::testing::Types<
        SetReduceTestCase<AVL_SetReduceSUM, 1>,
        SetReduceTestCase<AVL_SetReduceSUM, 10>,
        SetReduceTestCase<AVL_SetReduceSUM, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AVL_SetReduce, SetReduceTest, AVL_SetReduceTestTypes);