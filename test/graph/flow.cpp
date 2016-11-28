
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/flow.hpp"


namespace {
    
using namespace std;
using namespace ant;
using namespace ant::graph;


TEST(FordFulkerson, allin) {
    ifstream cin("./../data/graph/flow.txt");
    Count n_count, e_count;
    cin >> n_count >> e_count;
    DirEdgedGraphBuilder<Index, Index> builder(n_count);
    vector<int> ds(e_count);
    for (auto i = 0; i < e_count; ++i) {
        int from, to, dist;
        cin >> from >> to >> dist;
        ds[builder.add(from, to)] = dist;
    }
    auto gr = builder.build();
        
    FordFulkerson<decltype(gr), int> ff(gr, ds);
    
    int src, dst;
    cin >> src >> dst;
    int max_flow;
    cin >> max_flow;
    
    auto flow = ff.Compute(src, dst);
    ASSERT_EQ(max_flow, flow);
    
    cout << flow;
}

TEST(MaxBipartiteMatching, allin) {
    ifstream cin("./../data/graph/flow.txt");
    Count n_count, e_count;
    cin >> n_count >> e_count;
    DirEdgedGraphBuilder<Index, Index> builder(n_count);
    vector<int> ds(e_count);
    for (auto i = 0; i < e_count; ++i) {
        int from, to, dist;
        cin >> from >> to >> dist;
        ds[builder.add(from, to)] = dist;
    }
    auto gr = builder.build();
    
    FordFulkerson<decltype(gr), int> ff(gr, ds);
    
    int src, dst;
    cin >> src >> dst;
    int max_flow;
    cin >> max_flow;
    
    auto flow = ff.Compute(src, dst);
    ASSERT_EQ(max_flow, flow);
    
    cout << flow;

}


}