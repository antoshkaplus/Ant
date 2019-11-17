#pragma once

#include "bellman_ford.hpp"

namespace ant::graph {

// all pairs shortest paths
template <typename Graph, typename SupergraphBuilder, typename AllShortestPathsFunc, typename = std::enable_if_t<
        is_directed_v<Graph> &&
        std::is_arithmetic_v<typename Graph::EdgeValueType>>>
void Johnson(Graph& graph, SupergraphBuilder&& builder, AllShortestPathsFunc&& all_shortest_paths_func) {

    struct {
        bool negative_cycle;
        decltype(all_shortest_paths_func(std::declval<Graph>())) all_shortest_paths;
    } res;


    builder.Reset(graph);
    auto q = builder.AddVertex();
    for (auto& vertex : graph.vertices()) {
        builder.AddEdge(q, vertex, typename Graph::EdgeValueType{});
    }

    {
        auto subgraph = builder.Build();
        auto result = BellmanFord(subgraph, q);
        if (result.negative_cycle) {
            res.negative_cycle = true;
            return res;
        }

        for (auto& e : graph.edges()) {
            e.value += result.distance[e.source()] - result.distance[e.target()];
        }

        res.negative_cycle = false;
        res.all_shortest_paths = all_shortest_paths_func(graph);

        for (auto& e : graph.edges()) {
            e.value -= result.distance[e.source()] - result.distance[e.target()];
        }
    }
    return res;
}


}