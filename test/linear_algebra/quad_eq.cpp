
#include <fstream>
#include <array>
#include <functional>

#include "gtest/gtest.h"

#include "ant/linear_algebra/quad_eq.hpp"

namespace {

using namespace ant::linalg;

TEST(SolveQuadEq, _1) {
    auto r = SolveQuadEq(1, 1, 0);
    auto& arr = *r;
    ASSERT_EQ(arr[0], -1);
    ASSERT_EQ(arr[1], 0);
}

TEST(SolveQuadEq, _2) {
    auto r = SolveQuadEq(1, 1, 1);
    ASSERT_FALSE(r);
}

TEST(SolveQuadEq, _3) {
    auto r = SolveQuadEq(1, -2, 1);
    auto& arr = *r;
    ASSERT_EQ(arr[0], arr[1]);
}

}