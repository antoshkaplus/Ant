
#include "gtest/gtest.h"
#include "ant/graph/model/adj_vec/graph.hpp"
#include "ant/graph/policies.hpp"
#include "ant/graph/algo/dijkstra_components_base.hpp"
#include "ant/graph/algo/dijkstra_components.hpp"
#include "ant/graph/algo/dijkstra.hpp"
#include "ant/graph/algo/cluster.hpp"
#include "test/graph/graph.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::model::adj_vec;

TEST(Graph_Algo_DijkstraComponents, Small) {
    using EdgeValue = int32_t;
    Graph<ant::graph::policy::Policy<ant::graph::policy::EdgeValue<EdgeValue>>> g;

    decltype(g)::Mutator mutator(g);

    mutator.SetVertexCount(13);
    std::vector<std::tuple<Index, Index, EdgeValue>> edges = {
            {0, 1, 1}, {1, 2, 10}, {0, 2, 2},
            {3, 4, 7}, {4, 5, 8}, {3, 5, 9},
            {6, 7, 5}, {6, 8, 6}, {7, 8, 1},
            {9, 10, 2}, {9, 11, 3}, {11, 12, 4}, {10, 12, 5},
            // connections
            {1, 3, 4}, {5, 11, 1}, {2, 6, 2}, {8, 10, 4}

    };
    for (auto e : edges) {
        auto [i_1, i_2, value] = e;
        mutator.AddEdge(i_1, i_2, value);
    }

    graph::DijkstraComponentsBase algo(g);

    graph::Clustering clustering;
    clustering.vertex_cluster = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3};
    clustering.cluster_count = 4;

    algo.Compute(clustering);

    // between components
    ASSERT_EQ(algo.min_component_distance(0, 1), 4);
    ASSERT_EQ(algo.min_component_distance(0, 3), 12);
    ASSERT_EQ(algo.min_component_distance(1, 3), 1);
    ASSERT_EQ(algo.min_component_distance(1, 2), 9);

    // vertices inside component
    ASSERT_EQ(algo.Dist(0, 1), 1);
    ASSERT_EQ(algo.Dist(9, 11), 3);
    ASSERT_EQ(algo.Dist(11, 10), 5);

    graph::Dijkstra expected(g);

    // vertices different
    ASSERT_EQ(algo.Dist(0, 3), 5);
    ASSERT_EQ(std::get<0>(expected.Compute(0, 5)), algo.Dist(0, 5));
    ASSERT_EQ(std::get<0>(expected.Compute(0, 12)), algo.Dist(0, 12));
    ASSERT_EQ(std::get<0>(expected.Compute(0, 10)), algo.Dist(0, 10));

}

TEST(Graph_Algo_DijkstraComponents, Medium) {

    struct TestCase {
        Count node_count;
        Count edge_count;
        Count max_edge_value;
    };

    std::vector<TestCase> tests = {
            {7, 8, 5},
            {10, 10+2, 10},
            {20, 20+4, 10},
            {100, 100+10, 15},
            {300, 300+25, 25}
    };

    using Policy = ant::graph::policy::Policy<ant::graph::policy::EdgeValue<Count>>;

    std::default_random_engine rng;
    for (auto& t : tests) {
        auto graph = ant::graph::test::RandomConnectedEdgedValueGraph<Policy>(
                t.node_count, t.edge_count, t.max_edge_value, rng);

        graph::DijkstraComponentsBase tested(graph);

        auto cluster_count = std::max(1, static_cast<Count>(std::sqrt(CountVertices(graph))));
        auto clusters = graph::CenterClustering(graph).GenerateClusters(cluster_count, rng);
        tested.Compute(graph::Clustering{clusters, cluster_count});

        graph::Dijkstra expected(graph);

        for (auto v : graph.vertices()) {
            for (auto v_2 : graph.vertices()) {
                if (v.descriptor() > v_2.descriptor()) {
                    ASSERT_EQ(std::get<0>( expected.Compute(v.descriptor(), v_2.descriptor()) ),
                           tested.Dist(v.descriptor(), v_2.descriptor()));
                }
            }
        }
    }

}

}