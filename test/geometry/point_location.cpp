//
//  point_location.cpp
//  ANT
//
//  Created by Anton Logunov on 7/11/15.
//
//


#include "gtest/gtest.h"

#include "ant/geometry/d2.hpp"
#include "ant/geometry/triangle/point_location.hpp"

#include <fstream>

namespace {
    
    using namespace std;
    using namespace ant;
    using namespace ant::geometry::d2;
    using namespace ant::geometry::d2::i;
    using namespace ant::geometry::triangle;
    
    
    TEST(PointLocation, allin) {
        std::ifstream input("./../data/points_col.txt");
        bool b = input.good();
        cout << b << endl;
        int point_count;
        input >> point_count;
        std::vector<Point> points(point_count);
        for (int i = 0; i < point_count; ++i) {
            input >> points[i].x >> points[i].y;
        }
        auto is_inside = [&](Index index, const ant::geometry::triangle::Triangle& tr) {
            auto& ps = points;
            return PointInsideTriangle<i::Point>(ps[tr[0]], ps[tr[1]], ps[tr[2]]).IsInside(ps[index]);
        };
        auto is_on_segment = [&](Index v, Index s_0, Index s_1) {
            auto& ps = points;
            return Segment(ps[s_0], ps[s_1]).Lie(ps[v]);
        };
        Rectangle r = CircumRectangle(points);
        r.origin.x -= 1;
        r.origin.y -= 1;
        r.size.width += 2;
        r.size.height += 2;
        auto t = CircumTriangle(r);
        Index s = points.size();
        points.push_back(t[0]);
        points.push_back(t[1]);
        points.push_back(t[2]);
        ant::geometry::triangle::Triangle tt(s, s+1, s+2);
        PointLocation<decltype(is_inside), decltype(is_on_segment)> pl(tt, points.size(), is_inside, is_on_segment);
        for (Index i = 0; i < s; ++i) {
            pl.Insert(i);
            cout << "insert " << i << endl;
            pl.Print(cout);
        }
        pl.Print(cout);
    }
}