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
tuple<EdgedGraph<int, int>, vector<int>> RandomDirEdgedGraph(Count node_count, double completeness, Count max_edge_value) {

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

tuple<EdgedGraph<int, int>, vector<int>> RandomUndirEdgedGraph(Count node_count, double completeness, Count max_edge_value) {

    std::default_random_engine rng;
    std::uniform_real_distribution<> distr;
    std::uniform_int_distribution<> val_distr(1, max_edge_value);
    UndirEdgedGraphBuilder<int, int> builder(node_count);
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

    
TEST(GraphPathModule, allin_dir) {
    int testCount = 10;
    int nodeCount = 500;

    for (auto i = 0; i < testCount; ++i) {
        EdgedGraph<int, int> g;
        vector<int> vals;
        tie(g, vals) = RandomDirEdgedGraph(nodeCount, 0.5, 10000);

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
        std::default_random_engine rng;
        std::uniform_int_distribution<> distr(0, nodeCount-1);
        for (auto i = 0; i < nodeCount; ++i) {
            auto j = distr(rng);
            if (fw.Exists(i, j)) {
                ASSERT_EQ(fw.Dist(i, j), std::get<0>(dsp.Compute(i, j)));
            }
        }
    }
}


TEST(GraphPathModule, allin_undir) {
    int testCount = 10;
    int nodeCount = 500;

    for (auto i = 0; i < testCount; ++i) {
        EdgedGraph<int, int> g;
        vector<int> vals;
        tie(g, vals) = RandomUndirEdgedGraph(nodeCount, 0.1, 10000);

        FloydWarshall<int> fw(nodeCount);
        auto add = [&](Index from, Index to, Index edge) {
            fw.AddDirectedDist(from, to, vals[edge]);
            fw.AddDirectedDist(to, from, vals[edge]);
        };
        UndirGraphUtil::forEachIndexedEdge(g, add);
        fw.Compute();

        std::default_random_engine rng;
        std::uniform_int_distribution<> distr(0, nodeCount - 1);

        DijkstraShortestPath<decltype(g), int> dsp(g, vals);

        for (auto i = 0; i < nodeCount; ++i) {
            auto j = distr(rng);
            std::cout << i << std::endl;
            if (fw.Exists(i, j)) {
                ASSERT_EQ(fw.Dist(i, j), std::get<0>(dsp.Compute22(i, j)));
                //ASSERT_EQ(fw.Dist(i, j), std::get<0>(dsp.Compute22(i, j)));
                //ASSERT_EQ(fw.Dist(i, j), std::get<0>(dsp.Compute2(j, i)));
                //ASSERT_EQ(fw.Dist(i, j), std::get<0>(dsp.Compute22(j, i)));
            }
        }
    }
}


TEST(GraphPath, cluster) {
    EdgedGraph<int, int> g;
    vector<int> vals;

    std::default_random_engine rng;

    CenterClustering<decltype(g), int> clustering(g, vals);
    clustering.GenerateClusters(9, rng);

    clustering.partition(3, 9, rng);
}


}
