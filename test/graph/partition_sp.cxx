
#include "gtest/gtest.h"
#include "ant/graph/sp.hpp"
#include "ant/graph/partition_sp.hpp"
#include "test/graph/graph.hpp"

using namespace std;
using namespace ant;
using namespace ant::graph;
using namespace ant::graph::test;

using Builder = UndirEdgedGraphBuilder<Index, Index>;

void Add(Builder& builder, const vector<array<Index, 2>>& edges) {
    for (auto& e : edges) {
        builder.add(e[0], e[1]);
    }
}


TEST(Graph_Partition_SP, Constructor) {
    auto Test = [](Count node_count, const vector<array<Index, 2>>& edges) {
        UndirEdgedGraphBuilder<Index, Index> builder(node_count);
        Add(builder, edges);
        auto g = builder.build();
        vector<Int> edge_dist(g.edgeCount(), 1);
        Partition_SP<Int> sp(g, edge_dist);
        Println(cout, sp);
    };

    Test(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}}); // circle
    Test(5, {{0, 1}, {0, 2}, {0, 3}, {0, 4}}); // star
    Test(5, {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {3, 4}});
    Test(5, {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {3, 4}, {1, 2}});
    Test(6, {{2, 1}, {0, 2}, {2, 3}, {3, 5}, {3, 4}});
}

TEST(Graph_Partition_SP, Query_1_Length) {

    struct Case {
        Index source;
        Index target;
        Int dist;
    };

    auto Test = [](Count node_count, const vector<array<Index, 2>>& edges, const std::vector<Case>& cases) {
        UndirEdgedGraphBuilder<Index, Index> builder(node_count);
        Add(builder, edges);
        auto g = builder.build();
        vector<Int> edge_dist(g.edgeCount(), 1);
        Partition_SP<Int> sp(g, edge_dist);
        Println(cout, sp);

        for (auto& c : cases) {
            ASSERT_EQ(sp.Query(c.source, c.target).value(), c.dist);
        }
    };

    Test(8, {{0, 1}, {0, 2}, {2, 3}, {1, 3}, {3, 4},
             {4, 7}, {7, 6}, {6, 5}, {4, 5}},
         {{0, 6, 5}, {6, 0, 5}, {5, 4, 1}, {5, 7, 2}, {1, 4, 2}});
    Test(12, {{4,5}, {5,6}, {6,7}, {0,4}, {4,8}, {1,5}, {5,9}, {2,6}, {6,10}, {3,7}, {7,11}},
         {{0, 11, 5}, {2, 10, 2}, {8, 5, 2}});
    Test(15, {{0,1},{1,2},{2,3},{3,0},{0,4},{4,5},{5,6},{6,0},
              {6,7},{7,11},{11,12},{12,6},{7,10},{10,9},{9,8},{8,7},
              {5,14},{14,9},{2,13},{13,12}}, {{9,4,3}, {9,2,6}, {8,13,4}});
}

TEST(Graph_Partition_SP, Query) {

    struct Case {
        Index source;
        Index target;
        Int dist;
    };

    struct Edge {
        Index from;
        Index to;
        Int dist;
    };

    auto Test = [](Count node_count, const vector<Edge>& edges, const std::vector<Case>& cases) {

        UndirEdgedGraphBuilder<Index, Index> builder(node_count);
        vector<Int> edge_length;
        for (auto& e : edges) {
            builder.add(e.from, e.to);
            edge_length.push_back(e.dist);
        }
        auto g = builder.build();

        Partition_SP<Int> sp(g, edge_length);

        for (auto& c : cases) {
            ASSERT_EQ(sp.Query(c.source, c.target).value(), c.dist);
        }
    };

    Test(5, {{0,1,1},{0,1,2},{0,2,3},{1,2,1},{2,4,5},{2,3,2},{3,4,2}}, {{0,4,6},{0,2,2}});
    Test(12, {{0,1,1},{1,2,2},{2,3,3},{3,6,2},{2,4,3},{4,6,3},{2,5,4},{5,6,1},{6,7,1},{7,11,2},{11,10,5},{10,9,1},{8,9,6},{7,8,3}},
         {{0,9,17},{7,9,8},{0,6,8}});
}

struct Case {
    Count node_count;
    Count min_edges;
    Count max_edges;
    Count max_edge_weight;
};

template <typename RNG>
void Compare(const Case& c, Count queries, Count sample_count, RNG& rng) {
    for (auto i = 0; i < sample_count; ++i) {
        for (auto edge_count = c.min_edges; edge_count <= c.max_edges; ++edge_count) {
            auto test = RandomConnectedEdgedValueGraph(c.node_count, edge_count, c.max_edge_weight, rng);
            std::vector<Int> edge_weights(test.edge_weights.begin(), test.edge_weights.end());

            Dijkstra_SP<EdgedGraph<Index, Index>, Int> solver_1(test.graph, edge_weights);
            Partition_SP solver_2(test.graph, edge_weights);

            for (auto k = 0; k < queries; ++k) {
                uniform_int_distribution<> node_distr(0, c.node_count-1);
                auto i_1 = node_distr(rng);
                auto i_2 = node_distr(rng);

                auto r_1 = std::get<0>(solver_1.Compute(i_1, i_2));
                auto r_2 = *solver_2.Query(i_1, i_2);

                ASSERT_EQ(r_1, r_2);
            }
        }
    }
}


TEST(Graph_Partition_SP, CompareSmall) {

    std::default_random_engine rng;

    for (auto c : {Case{10, 9+1, 9+3, 5}, Case{20, 19+4, 19+6, 7}, Case{20, 19+4, 19+6, 10}})
    {
        Compare(c, c.node_count*c.node_count, c.node_count*c.node_count, rng);
    }
}

TEST(Graph_Partition_SP, CompareMedium) {

    std::default_random_engine rng;

    for (auto c : {Case{50, 50+5, 50+5, 40}})
    {
        Compare(c, c.node_count*c.node_count, 500, rng);
    }
}

TEST(Graph_Partition_SP, CompareLarge) {

    std::default_random_engine rng;

    for (auto c : {Case{200, 200+10, 200+10, 70}})
    {
        Compare(c, c.node_count*c.node_count, 20, rng);
    }
}

TEST(Graph_Partition_SP, CompareVeryLarge) {

    std::default_random_engine rng;

    for (auto c : {Case{1000, 1000+25, 1000+30, 180}})
    {
        Compare(c, 10*c.node_count, 20, rng);
    }
}


TEST(Graph_Partition_SP, CompareHuge) {

    std::default_random_engine rng;

    for (auto c : {Case{100'000, 100'000+25, 100'000+25, 10'000}})
    {
        Compare(c, 50'000, 1, rng);
    }
}