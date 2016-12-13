//
//  path.cpp
//  Ant
//
//  Created by Anton Logunov on 12/10/16.
//
//

#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/path.hpp"


namespace {
    
using namespace std;
using namespace ant;
using namespace ant::graph;


// completeness between 0 and 1 : ratio of edges
tuple<EdgedGraph<int, int>, vector<int>> RandomEdgedGraph(Count node_count, double completeness, Count max_edge_value) {

    std::default_random_engine rng;
    std::uniform_real_distribution<> distr;
    std::uniform_int_distribution<> val_distr(1, max_edge_value);
    DirEdgedGraphBuilder<int, int> builder(node_count);
    vector<int> edgeVals;
    for (auto i = 0; i < node_count; ++i) {
        for (auto j = 0; j < node_count; ++j) {
            if (i == j || distr(rng) > completeness) continue;
            builder.add(i, j);
            edgeVals.push_back(val_distr(rng));
        }
    } 
           
    return {builder.build(), edgeVals};       
}

    
    
TEST(GraphPathModule, allin) {
    int testCount = 10;
    int nodeCount = 500;
    
    for (auto i = 0; i < testCount; ++i) {
        EdgedGraph<int, int> g;
        vector<int> vals;
        tie(g, vals) = RandomEdgedGraph(nodeCount, 0.5, 10000);
        
        FloydWarshall<int> fw(nodeCount);
        auto add = [&](Index from, Index to, Index edge) {
            fw.AddDirectedDist(from, to, vals[edge]);
        }; 
        DirGraphUtil::forEachIndexedEdge(g, add);
        fw.Compute();
        
        DijkstraShortestPath<decltype(g), int> dsp(g, vals);
        for (auto i = 0; i < nodeCount; ++i) {
            vector<bool> visited;
            vector<int> dist;
            tie(dist, visited) = dsp.Compute(i);
            for (auto j = 0; j < nodeCount; ++j) {
                ASSERT_EQ(visited[j], fw.Exists(i, j));
                if (visited[j]) {
                    ASSERT_EQ(dist[j], fw.Dist(i, j)); 
                }
            }
        }
        
    }
}

}