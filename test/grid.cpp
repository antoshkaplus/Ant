//
//  grid.cpp
//  ANT
//
//  Created by Anton Logunov on 2/22/15.
//
//

#include <fstream>
#include <array>
#include <functional>

#include "gtest/gtest.h"

#include "ant/grid.hpp"

namespace {
    
using namespace ant;
using namespace ant::grid;



TEST(GridRegion, Iterator) {
    Count N = 11;
    Region t{0, 0, N, N};
    Count c = 0;
    for (auto p : t) {
        ASSERT_TRUE(t.hasInside(p));
        ++c;
    }
    ASSERT_TRUE(c == N*N);
}


TEST(GridRegion, Intersection) {
    Region r(0, 0, 2, 2);
    Region r_2(2, 0, 2, 2);
    ASSERT_FALSE(r.hasIntersection(r_2));
}

TEST(GridRegion, Constructors) {
    Region r{Position{0, 0}, Position{2, 2}};
}

    
}