
#include "gtest/gtest.h"

#include "ant/geometry/triangle/triangle.hpp"

#include <fstream>

namespace {

using namespace ant::geometry::triangle;

TEST(TriangleEdge, construction) {
    {
        Edge e(0, 1);
        ASSERT_EQ(e[0], 0);
        ASSERT_EQ(e[1], 1);
    }
    {
        Edge e(1, 0);
        ASSERT_EQ(e[0], 0);
        ASSERT_EQ(e[1], 1);
    }
}


}