
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"

#include "ant/geometry/d2.hpp"
#include "ant/geometry/segment_intersection.hpp"
#include "ant/geometry/segment_grid.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::geometry::d2;
using namespace ant::geometry::d2::i;


TEST(FindIntersectionPairs, simple) {
    std::ifstream input("./../data/2d_seg_inter_3.txt");
    vector<f::Point> ps;
    vector<array<Index, 2>> segs;
    Count segs_count;
    input >> segs_count;
    segs.resize(segs_count);
    ps.resize(2 * segs_count);
    for (int i = 0; i < segs_count; ++i) {
        input >> ps[2 * i].x >> ps[2 * i].y >> ps[2 * i + 1].x >> ps[2 * i + 1].y;
        segs[i] = {{2 * i, 2 * i + 1}};
    }
    Count inter_count;
    input >> inter_count;

    vector<f::Segment> point_segs(segs.size());
    transform(segs.begin(), segs.end(), point_segs.begin(), [&](auto &inds) {
        return f::Segment{ps[inds[0]], ps[inds[1]]};
    });

    auto inters = FindIntersectionPairs<f::Point>(point_segs);
    for (auto r : inters) {
        cout << r[0] << " " << r[1] << endl;
    }
}

}