
#include "gtest/gtest.h"
#include "ant/graph/partition.hpp"

using namespace std;
using namespace ant;
using namespace ant::graph;

void PrintPartition(const PartitionResult& res) {
    std::unordered_map<Index, std::unordered_set<Index>> ps;
    for (auto i = 0; i < res.node_partition.size(); ++i) {
        ps[res.node_partition[i]].insert(i);
    }
    Println(cout, "Partition:");
    for (auto& p : ps) {
        Println(cout, p.first, " [ ");
        Println(cout, std::vector<Index>(p.second.begin(), p.second.end()));
        Println(cout, " ] ");
    }
    Println(cout, res.node_degree);
}


TEST(Graph_Partition, All) {
    {
        UndirGraphBuilder<Index> builder(5);

        vector<array<Index, 2>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}};
        builder.Add(edges);
        auto g = builder.build();
        auto p = Partition(g);
        PrintPartition(p);
    }
    {
        UndirGraphBuilder<Index> builder(5);

        vector<array<Index, 2>> edges = {{0, 1}, {0, 2}, {0, 3}, {0, 4}};
        builder.Add(edges);
        auto g = builder.build();
        auto p = Partition(g);
        PrintPartition(p);
    }
    {
        UndirGraphBuilder<Index> builder(5);

        vector<array<Index, 2>> edges = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {3, 4}};
        builder.Add(edges);
        auto g = builder.build();
        auto p = Partition(g);
        PrintPartition(p);
    }
    {
        UndirGraphBuilder<Index> builder(5);

        vector<array<Index, 2>> edges = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {3, 4}, {1, 2}};
        builder.Add(edges);
        auto g = builder.build();
        auto p = Partition(g);
        PrintPartition(p);
    }
    {
        UndirGraphBuilder<Index> builder(6);

        vector<array<Index, 2>> edges = {{2, 1}, {0, 2}, {2, 3}, {3, 5}, {3, 4}};
        builder.Add(edges);
        auto g = builder.build();
        auto p = Partition(g);
        PrintPartition(p);
    }
}
