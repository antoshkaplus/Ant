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

#include "ant/grid.h"

namespace {
    
using namespace ant;
using namespace ant::grid;



TEST(GridRegion, Intersection) {
    Region r(0, 0, 2, 2);
    Region r_2(2, 0, 2, 2);
    ASSERT_FALSE(r.hasIntersection(r_2));
}

    
    
}