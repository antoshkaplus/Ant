

#include "gtest/gtest.h"

#include "ant/core/skip_list/skip_list_set.hpp"

using namespace ant;
using namespace std;

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Set<Value>,
        SkipListSet<Value>>::value>::type* = nullptr>
auto MakeSet(size_t capacity) {
    return SkipListSet<int>(capacity);
}

#include "test/core/set.hpp"

using SkipListSetTestTypes = ::testing::Types<
        SetTestCase<SkipListSet, 1>,
        SetTestCase<SkipListSet, 10>,
        SetTestCase<SkipListSet, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(SkipListSet, SetTest, SkipListSetTestTypes);

