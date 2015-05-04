//
//  convex_hull.cpp
//  ANT
//
//  Created by Anton Logunov on 4/24/15.
//
//

#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"

#include "ant/geometry/d2.h"

namespace {
    
using namespace ant;
using namespace ant::geometry::d2::i;
using namespace ant::geometry::d2;


TEST(ConvexHull, allin) {
    std::ifstream input("./../data/convex_hull.txt");
    int point_count;
    input >> point_count;
    std::vector<Point> points(point_count);
    for (int i = 0; i < point_count; ++i) {
        input >> points[i].x >> points[i].y;
    }
    auto inds = ConvexHull(points);
    for (auto i : inds) {
        std::cout << i << " " << points[i].x << " " << points[i].y << std::endl;
    } 
}
    
} // end anonymous namespace

