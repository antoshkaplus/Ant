#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/graph_3.hpp"
#include "bfs_3.hpp"


namespace ant::graph {

struct PartitionResult {
    std::vector<Index> node_partition;
    std::vector<Count> node_degree;
};

// cluster rays / segments / root. no much us except particular algorithms

// node -> cluster
inline PartitionResult Partition(const Graph<Index>& graph) {
    DisjointSet set(graph.nodeCount());
    std::vector<Count> degrees(graph.nodeCount());
    for (auto n = 0; n < graph.nodeCount(); ++n) {
        degrees[n] = graph.nextNodes(n).size();
    }
    // connect all rays
    for (auto n = 0; n < graph.nodeCount(); ++n) {
        Index p = n;
        // could be done by BFS too
        while (degrees[p] == 1) {
            auto& ns = graph.nextNodes(p);
            auto k = *std::find_if(ns.begin(), ns.end(), [&](auto k) { return set.is_separate(k, p); });
            set.Unite(k, p);
            --degrees[k];
            --degrees[p];
            p = k;
        }
    }

    assert(std::count(degrees.begin(), degrees.end(), 1) == 0);

    std::vector<Index> degree_nodes;
    for (auto n = 0; n < graph.nodeCount(); ++n) {
        if (degrees[n] == 2) {
            BFS_Prev(graph, n, [&](auto to, auto from) {
                if (!set.is_separate(to, from)) return BFS_Flow::Skip;
                if (degrees[to] > 2) return BFS_Flow::Skip;

                degree_nodes.clear();
                for (auto n : graph.nextNodes(to)) {
                    if (set.is_separate(to, n)) degree_nodes.push_back(n);
                }

                if (degree_nodes.size() == 2 && !set.is_separate(degree_nodes[0], degree_nodes[1])) {
                    --degrees[degree_nodes[0]];
                    --degrees[degree_nodes[1]];
                    --degrees[to];
                    --degrees[to];
                } else {
                    --degrees[to];
                    --degrees[from];
                }
                set.Unite(to, from);

                return BFS_Flow::Continue;
            });
        }
    }

    // now only roots left and we should consolidate the findings

    std::vector<Index> set_roots(graph.nodeCount());
    std::vector<bool> exists(graph.nodeCount(), false);
    for (auto n = 0; n < graph.nodeCount(); ++n) {
        set_roots[n] = set.root(n);
        exists[set_roots[n]] = true;
    }
    DecreaseClustering(set_roots, exists);

    return PartitionResult{std::move(set_roots), std::move(degrees)};
};

}