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

#include "ant/geometry/d2.hpp"
#include "ant/geometry/bentley_ottmann.hpp"
#include "ant/geometry/segment_grid.hpp"


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
    
TEST(BentleyOttmann, simple) {
    std::ifstream input("./../data/2d_seg_inter_2.txt");
    vector<f::Point> ps;
    vector<array<Index, 2>> segs;
    Count segs_count;
    input >> segs_count;
    segs.resize(segs_count);
    ps.resize(2*segs_count);
    for (int i = 0; i < segs_count; ++i) {
        input >> ps[2*i].x >> ps[2*i].y >> ps[2*i+1].x >> ps[2*i+1].y;
        segs[i] = {{2*i, 2*i+1}}; 
    }
    Count inter_count;
    input >> inter_count;
    
    auto func = [&](Index s_0, Index s_1) {
        return f::Intersection(
                   f::Segment{ps[segs[s_0][0]], ps[segs[s_0][1]]}, 
                   f::Segment{ps[segs[s_1][0]], ps[segs[s_1][1]]});
    };
    BentleyOttmann<f::Point, decltype(func)> bentley;
    auto inters = bentley.FindIntersections(ps, segs, func);
    for (auto p : inters) {
        cout << p[0] << " " << p[1] << endl;
    }  
}    


TEST(Segment, Lie) {
    Segment s;
    s.fst = {0, 0};
    s.snd = {5, 5};
    ASSERT_TRUE(s.Lie({1, 1}));
    ASSERT_FALSE(s.Lie({1, 3}));
}

TEST(PointInsideTriangle, allin) {
    i::Point ps[3] = {{0, 0}, {0, 10}, {10, 0}};
    i::Point on_edge = {0, 5};
    i::Point vertex = {0, 0};
    i::Point inside = {1, 2};
    i::Point outside = {-1, -1};
    using A = array<Int, 3>;
    for (auto& is : {A{{0, 1, 2}}, A{{1, 0, 2}}}) {
        PointTriangle<i::Point> nn{ps[is[0]], ps[is[1]], ps[is[2]]};
        ASSERT_FALSE(nn.IsInside(on_edge));
        ASSERT_FALSE(nn.IsInside(vertex));
        ASSERT_TRUE(nn.IsInside(inside));
        ASSERT_FALSE(nn.IsInside(outside));
        
        ASSERT_TRUE(nn.IsInsideOrLies(on_edge));
        ASSERT_TRUE(nn.IsInsideOrLies(vertex));
        ASSERT_TRUE(nn.IsInsideOrLies(inside));
        ASSERT_FALSE(nn.IsInsideOrLies(outside));
        
        ASSERT_TRUE(nn.Lies(on_edge));
        ASSERT_TRUE(nn.Lies(vertex));
        ASSERT_FALSE(nn.Lies(inside));
        ASSERT_FALSE(nn.Lies(outside));
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

template<typename P>
void OutputPoints(ostream& out, const vector<P>& ps) {
    out << ps.size() << endl;
    for (auto& p : ps) {
        out << p.x << " " << p.y << endl;
    }
}

void OutputSegments(ostream& out, const vector<array<Index, 2>>& segs) {
    out << segs.size() << endl;
    for (auto& s : segs) {
        out << s[0] << " " << s[1] << endl;
    }
}

template<typename P>
void OutputIntersections(ostream& out, 
                         const vector<array<Index, 2>>& inter_segs, 
                         const vector<P>& ps) {
    out << inter_segs.size() << endl;
    for (auto i = 0; i < inter_segs.size(); ++i) {
        out << inter_segs[i][0] << " " 
            << inter_segs[i][1] << " " 
            << ps[i].x << " " << ps[i].y << endl;
    }
}


TEST(BentleyOttmann, versatile) {
    uniform_int_distribution<> distr(0, 1000);
    default_random_engine rng;
    auto comp = [](const i::Point& p_0, const i::Point& p_1) {
        return p_0.x < p_1.x || (p_0.x == p_1.x && p_0.y < p_1.y);
    };
    set<i::Point, decltype(comp)> ps_set(comp);
    auto N = 30;
    for (int i = 0; i < N; ++i) {
        while (!ps_set.insert(i::Point{distr(rng), distr(rng)}).second);
    }
    vector<f::Point> ps(N);
    transform(ps_set.begin(), ps_set.end(), ps.begin(), [](const i::Point& p) {
        return f::Point(p.x, p.y);
    });
    shuffle(ps.begin(), ps.end(), rng);
    vector<array<Index, 2>> segs;
    for (auto i = 0; i < N/2; ++i) {
        segs.push_back({{2*i, 2*i+1}});
    }
    // do it from bin
    ofstream out("../output/out.txt");
    OutputPoints(out, ps);
    OutputSegments(out, segs);
    auto func = [&](Index s_0, Index s_1) {
        return f::Intersection(
                    f::Segment{ps[segs[s_0][0]], ps[segs[s_0][1]]}, 
                    f::Segment{ps[segs[s_1][0]], ps[segs[s_1][1]]});
    };
    BentleyOttmann<f::Point, decltype(func)> bentley;
    vector<array<Index, 2>> inters = bentley.FindIntersections(ps, segs, func);
    vector<f::Point> inter_ps(inters.size());
    transform(inters.begin(), inters.end(), inter_ps.begin(), [&](const array<Index, 2>& a) {
        return func(a[0], a[1]).first;
    });
    OutputIntersections(out, inters, inter_ps);
}

} // end anonymous namespace


namespace {
    
    using namespace std;
    using namespace ant;
    using namespace ant::geometry::d2;
    using namespace ant::geometry::d2::f;
    
    TEST(CircumCircle, allin) {
        f::Point p_0{0, 0};
        f::Point p_1{0, 10};
        f::Point p_2{5, 5};
        Circle c = CircumCircle(p_0, p_1, p_2);
        ASSERT_TRUE(c.center.Distance({0, 5}) < 1.e-8); 
        ASSERT_TRUE(abs(c.radius - 5) < 1.e-8);
    
//        f::Point p_0{-5, 5};
//        f::Point p_1{5, 5};
//        f::Point p_2{5, 5};
        
    }
    
}
 



