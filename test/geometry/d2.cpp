//
//  d2.cpp
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
#include "ant/geometry/bentley_ottmann.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::geometry::d2;
using namespace ant::geometry::d2::i;


TEST(ShoelaceFormula, allin) {
    std::ifstream input("./../data/convex_hull.txt");
    int point_count;
    input >> point_count;
    std::vector<Point> points(point_count);
    for (int i = 0; i < point_count; ++i) {
        input >> points[i].x >> points[i].y;
    }
    auto area = ShoelaceFormula(points);
    std::cout << area << std::endl; 
}
    
TEST(BentleyOttmann, allin) {
    std::ifstream input("./../data/2d_seg_inter_3.txt");
    vector<f::Point> ps;
    vector<pair<Index, Index>> segs;
    Count segs_count;
    input >> segs_count;
    segs.resize(segs_count);
    ps.resize(2*segs_count);
    for (int i = 0; i < segs_count; ++i) {
        input >> ps[2*i].x >> ps[2*i].y >> ps[2*i+1].x >> ps[2*i+1].y;
        segs[i] = {2*i, 2*i+1}; 
    }
    Count inter_count;
    input >> inter_count;
    
    auto func = [&](Index s_0, Index s_1) {
        return f::Intersection(
                   f::Segment{ps[segs[s_0].first], ps[segs[s_0].second]}, 
                   f::Segment{ps[segs[s_1].first], ps[segs[s_1].second]});
    };
    BentleyOttmann<f::Point, decltype(func)> bentley;
    auto inters = bentley.FindIntersections(ps, segs, func);
    for (auto p : inters) {
        cout << p.first << " " << p.second << endl;
    }  
}    

    
} // end anonymous namespace

