
#include <gtest/gtest.h>

#include "ant/simd/reverse.hpp"

template <typename TValue>
class Simd_Reverse_Test : public ::testing::Test {
public:
};

using MyTypes = ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>;
TYPED_TEST_SUITE(Simd_Reverse_Test, MyTypes);


TYPED_TEST(Simd_Reverse_Test, All) {
    for (auto N : {100, 1000, 10000, 17, 1, 0, 5, 10, 11, 13})
    {
        std::vector<TypeParam> arr_1(N);
        std::iota(arr_1.begin(), arr_1.end(), 0);
        auto arr_2 = arr_1;

        ant::simd::Reverse(arr_1.data(), arr_1.size());
        std::reverse(arr_2.begin(), arr_2.end());

        ASSERT_EQ(arr_1, arr_2);
    }
}
