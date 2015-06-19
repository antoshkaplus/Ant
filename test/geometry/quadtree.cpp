//
//  quadtree.cpp
//  ANT
//
//  Created by Anton Logunov on 6/19/15.
//
//

#include <numeric>

#include "gtest/gtest.h"

#include "ant/geometry/d2.hpp"
#include "ant/geometry/quadtree.hpp"

namespace {
    
    using namespace std;
    using namespace ant;
    using namespace ant::geometry::d2::f;
    using namespace ant::geometry::d2;
    
    TEST(Quadtree, allin) {
        Quadtree tree(Point{0,0}, Point{1, 1});
        vector<Point> ps;
        default_random_engine rng;
        Count N = 10000;
        ps.resize(N);
        for (auto& p : ps) {
            p = Point::Random(rng);
        }
        sort(ps.begin(), ps.end(), [](const Point& p_0, const Point& p_1) {
            return p_0.x < p_1.x || (p_0.x == p_1.x && p_0.y < p_1.y);
        });
        vector<Point> unips;
        unips.reserve(N);
        for (Index i = 0; i < ps.size(); ++i) {
            if (i > 0 && ps[i] == ps[i-1]) continue;
            unips.push_back(ps[i]);
        }
        for (Index i = 0; i < unips.size(); ++i) {
            tree.Insert(unips[i]);
        }
        vector<Point> near(unips.size());
        for (Index i = 0; i < unips.size(); ++i) {
            tree.Remove(unips[i]);
            near[i] = tree.NearestNeighbor(unips[i]);
            tree.Insert(unips[i]);
        }
    
        // need to compare with brute forse
        vector<Index> inds(unips.size());
        iota(inds.begin(), inds.end(), 0);
        for (Index i = 0; i < unips.size(); ++i) {
            swap(inds[i], inds.back());
            inds.pop_back();
            Index j = NearestPoint(unips, inds, unips[i]);
            ASSERT_EQ(unips[j], near[i]);
            inds.push_back(i);
            swap(inds[i], inds.back());
        }
    }
    
} // end anonymous namespace

