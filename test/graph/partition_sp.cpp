
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

    Test(8, {{0, 1}, {0, 2}, {2, 3}, {1, 3}, {3, 4},
             {4, 7}, {7, 6}, {6, 5}, {4, 5}},
         {{0, 6, 5}, {6, 0, 5}, {5, 4, 1}, {5, 7, 2}, {1, 4, 2}});
    Test(12, {{4,5}, {5,6}, {6,7}, {0,4}, {4,8}, {1,5}, {5,9}, {2,6}, {6,10}, {3,7}, {7,11}},
         {{0, 11, 5}, {2, 10, 2}, {8, 5, 2}});
    Test(15, {{0,1},{1,2},{2,3},{3,0},{0,4},{4,5},{5,6},{6,0},
              {6,7},{7,11},{11,12},{12,6},{7,10},{10,9},{9,8},{8,7},
              {5,14},{14,9},{2,13},{13,12}}, {{9,4,3}, {9,2,6}, {8,13,4}});
}

// try doing small graphs with random everything staying in line, compare with default algorithm