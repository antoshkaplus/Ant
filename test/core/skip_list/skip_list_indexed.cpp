
#include "ant/core/skip_list/skip_list_indexed.hpp"

using namespace ant;
using namespace std;

template <typename TestCase, typename Value,
        typename std::enable_if<std::is_same<typename TestCase::template Indexed<Value>,
                SkipListIndexed<Value>>::value>::type* = nullptr>
auto MakeIndexed(size_t capacity) {
    return SkipListIndexed<int>(capacity);
}

#include "test/core/indexed.hpp"

using SkipListIndexedTestTypes = ::testing::Types<
        IndexedTestCase<SkipListIndexed, 1>,
        IndexedTestCase<SkipListIndexed, 10>,
        IndexedTestCase<SkipListIndexed, 100>>;

// TODO FIX
// INSTANTIATE_TYPED_TEST_SUITE_P(SkipListIndexed, IndexedTest, SkipListIndexedTestTypes);

