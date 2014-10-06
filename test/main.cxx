//
//  main.cpp
//  Ant
//
//  Created by Anton Logunov on 4/24/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <chrono>
#include <random>


#include "ant/linear_algebra/matrix.h"
#include "ant/optimization.h"
#include "ant/simplex_method.h"
#include "ant/machine_learning.h"

using namespace ant;
using namespace d2::grid;
using namespace d2::i;
using namespace d2::f;
using namespace algebra;
using namespace opt::lp;
using namespace std;

void test_matrix() {
    Mat<int> m({{0 , 2, 2, 2, 1, 4}, 
        {17, 4, 2,13, 3, 1},
        {7 , 1, 1, 5, 1, 1}});
    sum(m);
    MatView<int> m_v = m.submat(0, 1, 3, 3);
    sum<int>(m_v);
    std::cout << m;
}

void test_standard_form() {
    Matrix m({{0 , 2, 2, 2, 1, 4}, 
        {17, 4, 2,13, 3, 1},
        {7 , 1, 1, 5, 1, 1}});
    
    StandardForm sf(m);
    m = sf.system();
    
    sf.pivot(0, 3);
    sf.pivot(1, 1);
    m = sf.system();
    
    for (auto r = 0; r < m.row_count(); ++r) {
        for (auto c = 0; c < m.col_count(); ++c) {
            cout << m(r, c) << " | ";
        }
        cout << endl;
    }
    /* result: -11, 2, 0, -5, 0,  1
     3, 2, 0,  3, 1, -1
     4,-1, 1,  2, 0,  2
     */
}


void test_canonical_form() {
    
    Matrix a({{-11,  2, -5,  1},
        {  3,  2,  3, -1},
        {  4, -1,  2,  2}});
    
    CanonicalForm cf(StandardForm(a), {3, 1}, {0, 2, 4});          
    cf.pivot(0, 1);
    cf.pivot(1, 2);
    a = cf.system();
    
    for (auto r = 0; r < a.row_count(); ++r) {
        for (auto c = 0; c < a.col_count(); ++c) {
            cout << a(r, c) << " | ";
        }
        cout << endl;
    }
    cout << cf;
}


void test_simplex_method() {
    Matrix m{
    {0 , 2, 1,  2, 1, 4},
    {17, 4, 2, 13, 3, 1},
    { 7, 1, 1,  5, 1, 1}};

    CanonicalForm cf{StandardForm(m)};
    //cout << cf;
    SimplexMethod sm;
    //cout << sm.solve(cf);
    // solution objective 4 x2 = 1, x1 = 2
    
    m = {
    {0 ,-2, -4,-3, -1},
    {12, 3,  1, 1,  4},
    { 7, 1, -3, 2,  3},
    {10, 2,  1, 3, -1}};
    cout << (cf = CanonicalForm(StandardForm(m), {4, 5, 6}, {0, 1, 2, 3}));
    cout << sm.solve(cf);
    
}

void test_simplex_method_hard() {
    Count b_count, non_b_count;
    cin >> b_count >> non_b_count;
    Matrix m(b_count+1, non_b_count+1);
    vector<Index> b_vec, non_b_vec;
    Index index;
    for (Index i = 0; i < b_count; ++i) {
        cin >> index;
        b_vec.push_back(index-1);
    }
    for (Index i = 0; i < non_b_count; ++i) {
        cin >> index; 
        non_b_vec.push_back(index-1);
    }
    //double number;
    for (Index i = 0; i < b_count; ++i) {
        cin >> m(i+1, 0);
    }
    for (auto r = 1; r < b_count+1; ++r) {
        for (auto c = 1; c < non_b_count+1; ++c) {
            cin >> m(r, c);
        }
    }
    for (auto i = 0; i < non_b_count+1; ++i) {
        cin >> m(0, i);
    }
    CanonicalForm cf(StandardForm(m), {4, 5, 6}, {0, 1, 2, 3});
    cout << "starting system:" << endl << cf << endl; 
    
    SimplexMethod sm;
    cf = sm.solve(cf);
    cout << "result system:" << endl << cf;
    
}



void test_grids(Count row_count, Count col_count, Count lookup_count) {
    using hrc = std::chrono::high_resolution_clock;
        
    vector<Position> lookup_positions;
    uniform_int_distribution<Index> row_distr(0, row_count-1);
    uniform_int_distribution<Index> col_distr(0, col_count-1);
    default_random_engine rng;
    for (auto i = 0; i < lookup_count; ++i) {
        lookup_positions.emplace_back(row_distr(rng), col_distr(rng));
    }
    
    double b;
    
    Grid<double> gr(row_count, col_count);
    SparceGrid<double> gr_sp(row_count, col_count);

    cout << "test setting" << endl;
    cout << "vector: ";
    auto start = hrc::now();
    for (auto &p : lookup_positions) {
        gr(p) = 0.;
    }
    cout << (hrc::now() - start).count() << endl;
    
    cout << "unordered map: ";
    start = hrc::now();
    for (auto &p : lookup_positions) {
        gr_sp(p) = 0.;
    }
    cout << (hrc::now() - start).count() << endl; 
    
    cout << "test getting" << endl;
    cout << "vector: ";
    start = hrc::now();
    for (auto &p : lookup_positions) {
        b = gr(p);
    }
    cout << (hrc::now() - start).count() << endl;
    
    cout << "unordered map: ";
    start = hrc::now();
    for (auto &p : lookup_positions) {
        b = gr_sp(p);
    }
    cout << (hrc::now() - start).count() << endl; 

}

void test_memcpy() {
    using hrc = std::chrono::high_resolution_clock;
    
    ant::Size sample_size = 100000000000;
    Index arr[100][100][4];
    Index buf[100][100][4];
    cout << "memcpy : ";
    auto start = hrc::now();
    for (Index i = 0; i < sample_size; ++i) {
        memcpy(arr, buf, sizeof(Index[100][100][4]));
    }
    cout << (hrc::now() - start).count() << endl;
    cout << "elementwise : ";
    Index i_0, i_1;
    start = hrc::now();
    for (Index i = 0; i < sample_size; ++i) {
        for (i_1 = 0; i_1 < 100; ++i_1) {
            for (i_0 = 0; i_0 < 100; ++i_0) {
                arr[i_0][i_1][0] = buf[i_0][i_1][0]; 
                arr[i_0][i_1][1] = buf[i_0][i_1][1];
                arr[i_0][i_1][2] = buf[i_0][i_1][2];
                arr[i_0][i_1][3] = buf[i_0][i_1][3];
            }
        }
    }
    cout << (hrc::now() - start).count() << endl;
}

void test_stack() {
    ant::stack<int> st;
    for (auto& s : st) {
    
    }
}

void test_point_methods() {
    Line ln(1, 2, 3);
    ant::d2::f::Point p(2, 2);
    double d = 10;
//    auto pp = circleLineIntersection(<#const ant::d2::f::Circle &circle#>, <#const ant::d2::f::Line &line#>)(ln, p, d);
//    cout << pp.first << ", " << pp.second;
}

void test_valued_segment_list() {
    d1::ValuedSegmentList<Int> vs;
//    vs.insert({8, 5}, 1);
//    vs.insert({0, 3}, 3);
//    vs.insert({3, 7}, 2);
//    vs.insert({6, 1}, 4);
//    vs.insert({10, 4}, 3);
//    vs.insert({5, 6}, 6);

    vs.insert({1,3}, 2);
    vs.insert({1,3}, 3);
    vs.insert({4,3}, 1);
    vs.insert({7,3}, 3);
    vs.insert({7-4, 4}, 2);
    vs.insert({4,3}, -10);
    //d1::operator<<(cout, vs);
    cout << vs;
}

void test_binary_decision_tree() {
    using V = vector<u_char>;
    using VV  = vector<V>;
    
    auto cond = [] (const V& raw, Index feature) {
        return raw[feature] > 128;
    };
    
    ml::binary_decision_tree<VV, decltype(cond), V> tree;
    tree.construct(VV(), cond, 10);
}


void test_max_empty_rect_finder() {
    Grid<bool> b =  {
                    {0, 1, 0, 0, 1, 1, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 1, 1, 1, 1, 0, 0},
                    {0, 0, 1, 0, 0, 1, 0, 1},
                    {1, 1, 1, 0, 0, 1, 0, 1}, 
                    {1, 1, 1, 1, 1, 1, 0, 1},
                    {1, 1, 0, 1, 0, 1, 1, 0},
                    {1, 1, 0, 0, 0, 1, 1, 0}};
    //cout << b;
    MaxEmptyRegionsFinder finder(b.size());
    auto regions = finder.find(b);
//    for (auto& r : regions) {
//        cout << r << "\n";
//    }
    MaxEmptyRegions max_regions(0, 0, 8, 8);
    max_regions.insertRegion(Region(0, 1, 1, 1));
    for (auto& r : max_regions.max_empty_regions()) {
        cout << r << "\n";
    } 
}

int main(int argc, char* argv[]) {
    // test_simplex_method_hard();
//    Count lookup_count = 1000000;
//    for (auto i : {100, 1000, 5000}) {
//        test_grids(i, i, lookup_count);
//    }
    //test_point_methods();
    //test_valued_segment_list();
    test_max_empty_rect_finder();
}






















