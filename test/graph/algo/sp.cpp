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
#include "ant/graph/algo/floyd_warshall.hpp"
#include "ant/graph/algo/dijkstra.hpp"
#include "test/graph/graph.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::graph;
using namespace ant::graph::test;


TEST(Graph_Algo, Directed_FloydWarshall_Dijkstra) {
    int testCount = 10;
    int nodeCount = 500;

    for (auto i = 0; i < testCount; ++i) {
        EdgedGraph<int, int> g;
        vector<int> vals;
        auto test = RandomDirEdgedGraph(nodeCount, 0.5, 10000);
        g = test.graph;
        vals = test.edge_weights;

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


TEST(Graph_Algo, Undirected_FloydWarshall_Dijkstra) {
    int testCount = 10;
    int nodeCount = 500;

    for (auto i = 0; i < testCount; ++i) {
        EdgedGraph<int, int> g;
        vector<int> vals;
        auto test = RandomUndirEdgedGraph(nodeCount, 0.1, 10000);
        g = test.graph;
        vals = test.edge_weights;

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

// TODO FIX
//TEST(GraphPath, cluster) {
//    EdgedGraph<int, int> g;
//    vector<int> vals;
//
//    std::default_random_engine rng;
//
//    CenterClustering<decltype(g), int> clustering(g, vals);
//    clustering.GenerateClusters(9, rng);
//
//    clustering.partition(3, 9, rng);
//}


}
