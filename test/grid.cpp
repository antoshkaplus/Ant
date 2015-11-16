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


Test ReadTest(istream& in) {
    char delim;
    string str;
    Count path_count;
    Test test;
    in >> test.origin >> delim >> test.target >> delim >> path_count;
    getline(in, str);
    for (auto i = 0; i < path_count; ++i) {
        // i just read end of line
        getline(in, str);
        auto ps = Split(str, ',');
        vector<Position> p(ps.size());
        for (auto i = 0; i < ps.size(); ++i) {
            stringstream ss(ps[i]);
            ss >> p[i];
        }
        test.paths.push_back(p);
    }
    return test;
}
 
 
 
TEST(Grid, BFS) {
    using Path = vector<Position>;
    string root = "../data/grid/raceway/";
    // should ve go with root
    ifstream in(root + "neighbors.txt");
    auto neighbors = ReadNeighbors(in);
    in.close();
    in.open(root + "raceway_01.txt");
    Grid<short> raceway;
    in >> raceway;
    in.close();
    in.open(root + "test_01.txt");
    ::Test test = ReadTest(in);
    in.close();
    auto is_neighbor = [&] (const Position& p, Direction d) {
        return neighbors[raceway[p]][d];
    };
    
    BFS<short, decltype(is_neighbor)> bfs;
    bfs.Init(raceway, is_neighbor);
    auto res = bfs.FindShortestPaths(test.origin, test.target);
    
    auto comp = [&](const Path& v_0, const Path& v_1) {
        if (v_0.size() < v_1.size()) {
            return true;
        } else if (v_0.size() == v_1.size()) {
            Position::TopLeftComparator cmp;
            for (auto i = 0; i < v_0.size(); ++i) {
                if (cmp(v_0[i], v_1[i])) {
                    return true;
                } if (v_0[i] != v_1[i]) {
                    return false;
                }
            }
        }
        return false;
    };
    
    // have to match test results and my results
    sort(res.begin(), res.end(), comp);
    sort(test.paths.begin(), test.paths.end(), comp);
    for (auto i = 0; i < res.size(); ++i) {
        ASSERT_EQ(res[i], test.paths[i]);
    }
}




}