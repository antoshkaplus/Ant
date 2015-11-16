//
//  grid.cpp
//  ANT
//
//  Created by Anton Logunov on 2/22/15.
//
//

#include <fstream>
#include <array>
#include <functional>

#include "gtest/gtest.h"

#include "ant/grid/grid.hpp"
#include "ant/grid/algorithms.hpp"

namespace {
    
using namespace ant;
using namespace ant::grid;



TEST(GridRegion, Iterator) {
    Count N = 11;
    Region t{0, 0, N, N};
    Count c = 0;
    for (auto p : t) {
        ASSERT_TRUE(t.hasInside(p));
        ++c;
    }
    ASSERT_TRUE(c == N*N);
}


TEST(GridRegion, Intersection) {
    Region r(0, 0, 2, 2);
    Region r_2(2, 0, 2, 2);
    ASSERT_FALSE(r.hasIntersection(r_2));
}

TEST(GridRegion, Constructors) {
    Region r{Position{0, 0}, Position{2, 2}};
}



vector<array<bool, 4>> ReadNeighbors(istream& in) {
    vector<array<bool, 4>> res;
    string str;
    for (;;) {
        getline(in, str);
        if (str.empty()) break;
        if (str[0] == ';') continue;
        auto mid = str.find(':');
        auto index_str = str.substr(0, mid);
        int i = atoi(index_str.c_str());
        if (i >= res.size()) {
            res.resize(i+1);
        }
        auto values_str = str.substr(mid+1);
        // have to trim this shit
        auto values = Split<4>(values_str, ',');
        auto func = [&](std::string str) {
            str = Trim(str);
            if (str == "T") return true;
            if (str == "F") return false;
            throw std::runtime_error("Unknown type");
        };
        transform(values.begin(), values.end(), res[i].begin(), func);
    }
    return res;
}

struct Test {
    Position origin;
    Position target;
    vector<vector<Position>> paths;
};


//Test ReadTest(istream& in) {
//    char delim;
//    Count path_count;
//    Test test;
//    in >> test.origin >> delim >> test.target >> delim >> path_count;
//    for (auto i = 0; i < path_count; ++i) {
//        string str;
//        getline(in, str);
//        Split<3>(str, ',');
//    }
//    return test;
//}
 
 
 
TEST(Grid, BFS) {
    // should ve go with root
    ifstream in("../data/grid/raceway/neighbors.txt");
    auto neighbors = ReadNeighbors(in);
    in.open("../../data/grid/raceway/raceway_01.txt");
    Grid<short> g;
    in >> g;
    //ReadTest(in);
    
    
    
}




}