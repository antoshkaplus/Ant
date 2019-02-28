
#include "gtest/gtest.h"

#include "ant/core/core.hpp"

using namespace ant;
using namespace std;

TEST(ParseDuration, Valid) {
    ASSERT_EQ(ParseDuration("1h"), ToDuration(std::chrono::hours(1)));
    ASSERT_EQ(ParseDuration(" 3h 89m 890ms"), ToDuration(
            std::chrono::hours(3) + std::chrono::minutes(89) + std::chrono::milliseconds(890)));
}

TEST(ParseDuration, Invalid) {
    EXPECT_ANY_THROW(ParseDuration("hh"));
    EXPECT_ANY_THROW(ParseDuration(" h"));
    EXPECT_ANY_THROW(ParseDuration(" h4"));
    EXPECT_ANY_THROW(ParseDuration("3h  h"));
    EXPECT_ANY_THROW(ParseDuration("4h 2us 323"));
}