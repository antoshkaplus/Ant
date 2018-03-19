//
//  range_count.cpp
//  Ant
//
//  Created by Anton Logunov on 8/14/16.
//
//

#include "gtest/gtest.h"

#include "ant/core/range_count.hpp"

using namespace ant;
using namespace std;

using P = pair<int, int>;
int len(const P& p) {
    return p.second - p.first + 1;
}


TEST(RangeCount, correctness_1) {
    int first = 0;
    int last = 50000;
    int sz = last+1;
    RangeCount counter(sz);
    vector<P> intervals(sz);
    for (auto i = 0; i < sz; ++i) {
        intervals[i] = {i, i};
    }
    default_random_engine rng;
    shuffle(intervals.begin(), intervals.end(), rng);
    for (auto& i : intervals) {
        counter.AddToRight(i.first, i.second, 1);
    }
    uniform_int_distribution<> distr(first, last);
    for (int i = 0; i < sz; ++i) {
        int x_1 = distr(rng);
        int x_2 = distr(rng);
        auto p = std::minmax(x_1, x_2);
        int count = counter.Count(p.first, p.second);
        ASSERT_EQ(count, len(p));
    }
}


TEST(RangeCount, correctness_2) {
    int first = 0;
    int last = 50000;
    int sz = last+1;
    RangeCount counter(sz);
    vector<P> intervals(sz);
    uniform_int_distribution<> distr(first, last);
    default_random_engine rng;
    for (auto& i : intervals) {
        int x_1 = distr(rng);
        int x_2 = distr(rng);
        i = std::minmax(x_1, x_2);
        int count = counter.Count(i.first, i.second);
        counter.AddToRight(i.first, i.second, len(i) - count);
    }
    for (auto& i : intervals) {
        auto count = counter.Count(i.first, i.second);
        ASSERT_EQ(count, len(i));
    }
}







