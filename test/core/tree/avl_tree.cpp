
#include "gtest/gtest.h"

#include "ant/core/tree/avl_set.hpp"
#include "ant/core/tree/avl_map.hpp"
#include "ant/core/tree/avl_set_reduce.hpp"

using namespace ant;
using namespace ant::core::tree;
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
