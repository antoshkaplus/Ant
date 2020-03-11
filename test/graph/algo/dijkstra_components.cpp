
#include "gtest/gtest.h"
#include "ant/graph/model/adj_vec/graph.hpp"
#include "ant/graph/policies.hpp"
#include "ant/graph/algo/dijkstra_components_base.hpp"
#include "ant/graph/algo/dijkstra.hpp"
#include "ant/graph/algo/cluster.hpp"


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

    ASSERT_EQ(algo.min_component_distance(0, 1), 4);
    ASSERT_EQ(algo.min_component_distance(0, 3), 12);
    ASSERT_EQ(algo.min_component_distance(1, 3), 1);
    ASSERT_EQ(algo.min_component_distance(1, 2), 9);

    graph::Dijkstra expected(g);

    ASSERT_EQ(algo.Dist(0, 1), 1);
    ASSERT_EQ(algo.Dist(9, 11), 3);

    ASSERT_EQ(algo.Dist(0, 3), 5);



    ASSERT_EQ(algo.Dist(0, 5), 14) << std::get<0>(expected.Compute(0, 5));
    ASSERT_EQ(algo.Dist(0, 12), 19);
    ASSERT_EQ(algo.Dist(0, 10), 14);

}


}