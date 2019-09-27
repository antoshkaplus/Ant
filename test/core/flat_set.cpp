

#include "gtest/gtest.h"

#include "ant/core/flat_set.hpp"

using namespace ant;
using namespace std;

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Set<Value>,
                FlatSet<Value>>::value>::type* = nullptr>
auto MakeSet(size_t capacity) {
    FlatSet<int> res;
    res.Reserve(capacity);
    return res;
}

#include "test/core/set.hpp"

using FlatSetTestTypes = ::testing::Types<
        SetTestCase<FlatSet, 1>,
        SetTestCase<FlatSet, 10>,
        SetTestCase<FlatSet, 100>>;

INSTANTIATE_TYPED_TEST_SUITE_P(FlatSet, SetTest, FlatSetTestTypes);

