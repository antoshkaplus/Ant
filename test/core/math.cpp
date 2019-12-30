
#include "gtest/gtest.h"

#include "ant/core/math.hpp"


using namespace ant;
using namespace std;


TEST(core_is_power_of_2, _1) {
    ASSERT_FALSE(is_power_of_2(9));
    ASSERT_TRUE(is_power_of_2(16));
    ASSERT_TRUE(is_power_of_2(256));
    ASSERT_TRUE(is_power_of_2(1));
    ASSERT_FALSE(is_power_of_2(112));
}

