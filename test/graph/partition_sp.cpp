
#include "gtest/gtest.h"
#include "ant/graph/partition_sp.hpp"

using namespace std;
using namespace ant;
using namespace ant::graph;

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

TEST(Graph_Partition_SP, Query) {

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



}