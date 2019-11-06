
#include <fstream>
#include <array>
#include <functional>
#include <iostream>
#include <ant/geometry/segment_intersection.hpp>

#include "gtest/gtest.h"

#include "ant/geometry/d2.hpp"
#include "ant/geometry/bentley_ottmann.hpp"
#include "ant/geometry/segment_grid.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::geometry::d2;
using namespace ant::geometry::d2::i;

void OutputSegments(ostream& out, const vector<f::Segment>& segs) {
    out << segs.size() << endl;
    for (auto& s : segs) {
        out << s.p_0.x << " " << s.p_0.y << " " << s.p_1.x << " " << s.p_1.y << endl;
    }
}

void OutputNestedVectorsIndexes(ostream& out, const vector<f::Segment>& segs, const NestedVectors<const f::Segment*>& vs) {
    for (auto i = 0; i < vs.size(); ++i) {
        auto r = vs.range(i);
        for (auto k = r.first; k < r.second; ++k) {
            cout << vs[k] - segs.data() << " ";
        }
        cout << endl;
    }
}

void Sort(std::vector<std::array<Index, 2>>& ps) {
    for_each(ps.begin(), ps.end(), [](auto& s) {
        if (s[0] > s[1]) swap(s[0], s[1]);
    });
    sort(ps.begin(), ps.end());
}

std::vector<std::array<Index, 2>> ConvertToIndexPairs(const std::vector<f::Segment>& segs, const NestedVectors<const f::Segment*>& vs) {
    std::vector<std::array<Index, 2>> arr;
    for (auto i = 0; i < vs.size(); ++i) {
        auto r = vs.range(i);

        auto& s_first = *vs[r.first];
        for (auto k = r.first+1; k < r.second; ++k) {
            assert(Intersection(s_first, *vs[k]).second);
        }

        for (auto k_1 = r.first; k_1 < r.second; ++k_1) {
            for (auto k_2 = k_1 + 1; k_2 < r.second; ++k_2) {
                auto q_1 = Index(vs[k_1] - segs.data());
                auto q_2 = Index(vs[k_2] - segs.data());
                assert(q_1 != q_2);
                arr.push_back({{q_1, q_2}});
            }
        }
    }
    return arr;
}


TEST(BentleyOttmann, simple) {
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
        auto& s_first = *inters[r.first];
        for (auto k = r.first+1; k < r.second; ++k) {
            assert(Intersection(s_first, *inters[k]).second);
        }
    }
}

// TODO FIX
//
//TEST(BentleyOttmann, random) {
//    std::default_random_engine rng;
//    std::uniform_real_distribution<double> distr;
//
//    std::vector<Count> sizes = {10, 50, 100, 200};
//    for (auto s : sizes) {
//        std::vector<f::Segment> segs(s);
//        for_each(segs.begin(), segs.end(), [&](auto& s) {
//            s.p_0.x = distr(rng);
//            s.p_0.y = distr(rng);
//            s.p_1.x = distr(rng);
//            s.p_1.y = distr(rng);
//        });
//
//        ofstream out("temp/out.txt");
//        OutputSegments(out, segs);
//        out.close();
//
//        BentleyOttmann<f::Point, f::Segment> bentley;
//        auto inters = bentley.FindIntersections(segs);
//        vector<array<Index, 2>> bentley_pairs = ConvertToIndexPairs(segs, inters);
//        Sort(bentley_pairs);
//
//        auto pairs = FindIntersectionPairs<f::Point>(segs);
//        Sort(pairs);
//
//        ASSERT_EQ(bentley_pairs, pairs);
//    }
//}
//
//TEST(BentleyOttmann, horizontal) {
//    std::default_random_engine rng;
//    std::uniform_real_distribution<double> distr;
//
//    auto N = 10;
//    auto TESTS = 100;
//
//    for (auto t = 0; t < TESTS; ++t) {
//        std::vector<f::Segment> segs(2*N);
//        for_each(segs.begin(), segs.end(), [&](auto& s) {
//            s.p_0.x = distr(rng);
//            s.p_0.y = distr(rng);
//            s.p_1.x = distr(rng);
//            s.p_1.y = distr(rng);
//        });
//        for (auto i = 0; i < N; ++i) {
//            segs[i].p_0.y = segs[i].p_1.y;
//        }
//
//        ofstream out("temp/out.txt");
//        OutputSegments(out, segs);
//        out.close();
//
//        BentleyOttmann<f::Point, f::Segment> bentley;
//        auto inters = bentley.FindIntersections(segs);
//        vector<array<Index, 2>> bentley_pairs = ConvertToIndexPairs(segs, inters);
//        Sort(bentley_pairs);
//
//        auto pairs = FindIntersectionPairs<f::Point>(segs);
//        Sort(pairs);
//
//        ASSERT_EQ(bentley_pairs, pairs);
//    }
//}

// TEST IDEAS:
//
// to try edge cases where there are some vertical-horizontal lines
// +
// three lines intersect at one point
// +
// all intersections should correspond to brute force algorithm one to one
// +
// check ability to work on many more points


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
