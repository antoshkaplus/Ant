//
//  range_minmax.cpp
//  Ant
//
//  Created by Anton Logunov on 2/20/16.
//
//

#include "gtest/gtest.h"

#include "ant/core/range_minmax.hpp"
#include "ant/core/range_min_interval.hpp"
#include "ant/core/seg_tree.hpp"

using namespace ant;
using namespace std;


int RangeMin(const vector<int>& vs, int from, int n) {
    return *min_element(vs.begin()+from, vs.begin()+from+n);
}

int RangeMinInterval(const vector<int>& vs, int from, int n) {
    int min_sum = std::numeric_limits<int>::max();
    for (int i = from; i < from+n; ++i) {
        for (int j = i; j < from+n; ++j) {
            int sum = 0;
            for (int k = i; k <= j; ++k) {
                sum += vs[k];
            }
            if (min_sum > sum) {
                min_sum = sum;
            }
        }
    }
    return min_sum;
}

// both included
int QuerySum(const vector<int>& vs, int from, int to) {
    return accumulate(vs.begin()+from, vs.begin()+to+1, 0);
}


TEST(RangeMinMax, all_in) {
    int num_tests = 1000;
    int num_query = 10;
    default_random_engine rng;
    for (int i = 1; i <= num_tests; ++i) {
        uniform_int_distribution<> vs_val_distr(1, i);
        vector<int> vs(i);
        for (auto& v : vs) v = vs_val_distr(rng);

        RangeMinimum<int> range_min(vs);

        uniform_int_distribution<> vs_index_distr(0, vs.size()-1);
        for (auto j = 0; j < num_query; ++j) {
            auto from = vs_index_distr(rng);
            auto to = vs_index_distr(rng);
            if (from > to) {
                swap(from, to);
            }
            int n = to-from+1;
            //if (from == 5 && n == 4) {
                int resExpected = RangeMin(vs, from, n);

                int res = range_min.Minimum(from, n);
                ASSERT_EQ(res, resExpected);

            //}

        }
    }
}

TEST(RangeMinInterval, all_in) {
    int num_tests = 1000;
    int num_query = 10;
    default_random_engine rng;
    for (int i = 1; i <= num_tests; ++i) {
        uniform_int_distribution<> vs_val_distr(-i, i);
        vector<int> vs(i);
        for (auto& v : vs) v = vs_val_distr(rng);

        RangeMinimumInterval<int> range_min_interval(vs);

        uniform_int_distribution<> vs_index_distr(0, vs.size()-1);
        for (auto j = 0; j < num_query; ++j) {
            auto from = vs_index_distr(rng);
            auto to = vs_index_distr(rng);
            if (from > to) {
                swap(from, to);
            }
            int n = to-from+1;
            int resExpected = RangeMinInterval(vs, from, n);
            int res = range_min_interval.MinimumInterval(from, n);
            ASSERT_EQ(res, resExpected);
        }
    }
}

// TODO FIX
//TEST(RangeMinInterval, update) {
//    int num_tests = 1000;
//    int num_query = 10;
//    default_random_engine rng;
//    for (int i = 1; i <= num_tests; ++i) {
//        uniform_int_distribution<> vs_val_distr(-i, i);
//        vector<int> vs(i);
//        for (auto& v : vs) v = vs_val_distr(rng);
//
//        RangeMinimumInterval<int> range_min_interval(vs);
//
//        uniform_int_distribution<> vs_index_distr(0, vs.size()-1);
//        for (auto j = 0; j < num_query; ++j) {
//            auto from = vs_index_distr(rng);
//            auto to = vs_index_distr(rng);
//            if (from > to) {
//                swap(from, to);
//            }
//            int n = to-from+1;
//            uniform_int_distribution<> update_distr(from, to);
//            auto idx = update_distr(rng);
//            vs[idx] = vs_val_distr(rng);
//            range_min_interval.Update(idx, vs[idx]);
//            int resExpected = RangeMinInterval(vs, from, n);
//            int res = range_min_interval.MinimumInterval(from, n);
//            ASSERT_EQ(res, resExpected);
//        }
//    }
//}

TEST(SegTree, allin) {
    int num_tests = 1000;
    int num_query = 10;
    default_random_engine rng;
    auto sum = [](int v_1, int v_2) {
        return v_1 + v_2;
    };
    for (int i = 1; i <= num_tests; ++i) {
        uniform_int_distribution<> vs_val_distr(1, i);
        vector<int> vs(i);
        for (auto& v : vs) v = vs_val_distr(rng);

        SegTree<int, decltype(sum)> tree(vs, sum);

        uniform_int_distribution<> vs_index_distr(0, vs.size()-1);
        for (auto j = 0; j < num_query; ++j) {
            auto from = vs_index_distr(rng);
            auto to = vs_index_distr(rng);
            if (from > to) {
                swap(from, to);
            }
            uniform_int_distribution<> update_distr(from, to);
            auto idx = update_distr(rng);
            vs[idx] = vs_val_distr(rng);
            tree.update(idx, vs[idx]);
            int resExpected = QuerySum(vs, from, to);
            int res = tree.Query(from, to);
            ASSERT_EQ(res, resExpected);
        }
    }
}


