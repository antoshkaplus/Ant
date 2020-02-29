#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/algo/floyd_warshall.hpp"
#include "ant/graph/algo/dijkstra.hpp"
#include "ant/graph/graph.hpp"
#include "test/graph/graph.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::graph;
using namespace ant::graph::test;


TEST(Graph_Algo_FloydWarshall_Dijkstra, Directed) {
    int testCount = 10;
    int nodeCount = 500;

    using Policy = policy::Policy<policy::Directed, policy::EdgeValue<int>>;
    for (auto i = 0; i < testCount; ++i) {
        Graph g = RandomEdgeValueGraph<Policy>(nodeCount, 0.5, 10000);

        FloydWarshall<int> fw(nodeCount);

        for (auto&& edge : g.edges()) {
            fw.AddDirectedDist(edge.from().descriptor(), edge.to().descriptor(), edge.value());
        }

        fw.Compute();

        Dijkstra<decltype(g)> dsp(g);
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

TEST(Graph_Algo_FloydWarshall_Dijkstra, Undirected) {
    int testCount = 10;
    int nodeCount = 500;

    using Policy = policy::Policy<policy::EdgeValue<int>>;
    for (auto t = 0; t < testCount; ++t) {
        Graph g = RandomEdgeValueGraph<Policy>(nodeCount, 0.1, 10000);

        FloydWarshall<int> fw(nodeCount);

        for (auto&& edge : g.edges()) {
            auto&& vs = edge.vertices();
            fw.AddDirectedDist(vs[0].descriptor(), vs[1].descriptor(), edge.value());
            fw.AddDirectedDist(vs[1].descriptor(), vs[0].descriptor(), edge.value());
        }

        fw.Compute();

        std::default_random_engine rng;
        std::uniform_int_distribution<> distr(0, nodeCount - 1);

        Dijkstra<decltype(g)> dsp(g);

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
