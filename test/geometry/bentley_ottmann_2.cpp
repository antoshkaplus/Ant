
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"

#include "ant/geometry/d2.hpp"
#include "ant/geometry/bentley_ottmann_2.hpp"
#include "ant/geometry/segment_grid.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::geometry::d2;
using namespace ant::geometry::d2::i;


TEST(BentleyOttmann_2, simple) {
    std::ifstream input("./../data/2d_seg_inter_0.txt");
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

    auto func = [&](Index s_0, Index s_1) {
        return f::Intersection(
                f::Segment{ps[segs[s_0][0]], ps[segs[s_0][1]]},
                f::Segment{ps[segs[s_1][0]], ps[segs[s_1][1]]});
    };
    BentleyOttmann<f::Point, f::Segment> bentley;


    vector<f::Segment> point_segs(segs.size());
    transform(segs.begin(), segs.end(), point_segs.begin(), [&](auto &inds) {
        return f::Segment{ps[inds[0]], ps[inds[1]]};
    });

    auto inters = bentley.FindIntersections(point_segs);
    for (auto i = 0; i < inters.size(); ++i) {
        auto r = inters.range(i);
        for (auto k = r.first; k < r.second; ++k) {
            cout << inters[k] - point_segs.data() << " ";
        }
        cout << endl;
    }
}

}

//
//TEST(BentleyOttmann, convex_polygon) {
//    std::ifstream input("./../data/convex_hull.txt");
//    int point_count;
//    input >> point_count;
//    std::vector<f::Point> ps(point_count);
//    for (int i = 0; i < point_count; ++i) {
//        input >> ps[i].x >> ps[i].y;
//    }
//    auto inds = ConvexHull(ps);
//    vector<array<Index, 2>> segs;
//    for (auto i = 0; i < inds.size(); ++i) {
//        segs.push_back({{inds[i], inds[(i+1)%inds.size()]}});
//    }
//    auto func = [&](Index s_0, Index s_1) {
//        return f::Intersection(
//                               f::Segment{ps[segs[s_0][0]], ps[segs[s_0][1]]},
//                               f::Segment{ps[segs[s_1][0]], ps[segs[s_1][1]]});
//    };
//    BentleyOttmann<f::Point, decltype(func)> bentley;
//    bentley.FindIntersections(ps, segs, func);
//}
//
//template<typename P>
//void OutputPoints(ostream& out, const vector<P>& ps) {
//    out << ps.size() << endl;
//    for (auto& p : ps) {
//        out << p.x << " " << p.y << endl;
//    }
//}
//
//void OutputSegments(ostream& out, const vector<array<Index, 2>>& segs) {
//    out << segs.size() << endl;
//    for (auto& s : segs) {
//        out << s[0] << " " << s[1] << endl;
//    }
//}
//
//template<typename P>
//void OutputIntersections(ostream& out,
//                         const vector<array<Index, 2>>& inter_segs,
//                         const vector<P>& ps) {
//    out << inter_segs.size() << endl;
//    for (auto i = 0; i < inter_segs.size(); ++i) {
//        out << inter_segs[i][0] << " "
//            << inter_segs[i][1] << " "
//            << ps[i].x << " " << ps[i].y << endl;
//    }
//}
//
//
//TEST(BentleyOttmann, versatile) {
//    uniform_int_distribution<> distr(0, 1000);
//    default_random_engine rng;
//    auto comp = [](const i::Point& p_0, const i::Point& p_1) {
//        return p_0.x < p_1.x || (p_0.x == p_1.x && p_0.y < p_1.y);
//    };
//    set<i::Point, decltype(comp)> ps_set(comp);
//    auto N = 30;
//    for (int i = 0; i < N; ++i) {
//        while (!ps_set.insert(i::Point{distr(rng), distr(rng)}).second);
//    }
//    vector<f::Point> ps(N);
//    transform(ps_set.begin(), ps_set.end(), ps.begin(), [](const i::Point& p) {
//        return f::Point(p.x, p.y);
//    });
//    shuffle(ps.begin(), ps.end(), rng);
//    vector<array<Index, 2>> segs;
//    for (auto i = 0; i < N/2; ++i) {
//        segs.push_back({{2*i, 2*i+1}});
//    }
//    // do it from bin
//    ofstream out("../output/out.txt");
//    OutputPoints(out, ps);
//    OutputSegments(out, segs);
//    auto func = [&](Index s_0, Index s_1) {
//        return f::Intersection(
//                f::Segment{ps[segs[s_0][0]], ps[segs[s_0][1]]},
//                f::Segment{ps[segs[s_1][0]], ps[segs[s_1][1]]});
//    };
//    BentleyOttmann<f::Point, decltype(func)> bentley;
//    vector<array<Index, 2>> inters = bentley.FindIntersections(ps, segs, func);
//    vector<f::Point> inter_ps(inters.size());
//    transform(inters.begin(), inters.end(), inter_ps.begin(), [&](const array<Index, 2>& a) {
//        return func(a[0], a[1]).first;
//    });
//    OutputIntersections(out, inters, inter_ps);
//}
//
//} // end anonymous namespace
//
