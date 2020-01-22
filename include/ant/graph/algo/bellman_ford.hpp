#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/graph.hpp"

namespace ant::graph {

// search shortest distances from one to all
template <typename Graph, typename = std::enable_if_t<
        is_directed_v<Graph> &&
        is_vertex_descriptor_index_v<Graph> &&
        std::is_signed_v<Graph::VertexDescriptor> &&
        std::is_arithmetic_v<typename Graph::EdgeValue>>>
auto BellmanFord(const Graph& G, typename Graph::VertixDescriptor source) {

    std::vector<typename Graph::EdgeValue> distance(G.vertex_count(), std::numeric_limits<typename Graph::EdgeValue>::max());
    std::vector<typename Graph::VertixDescriptor> predecessor(G.vertex_count(), -1);

    distance[source] = typename Graph::EdgeValue {};

    for (typename Graph::VertixDescriptor i = 0; i < G.vertex_count()-1; ++i) {
        for (auto& e : G.edges()) {
            // member functions are used as fetching all members all the time can be costly
            // especially if not all used all the time
            if (distance[e.source()] + e.value() < distance[e.target()]) {
                distance[e.target()] = distance[e.source()] + e.value();
                predecessor[e.target()] = e.source();
            }
        }
    }

    bool negative_cycle = false;
    for (auto& e : G.edges()) {
        if (distance[e.source()] + e.value() < distance[e.target()]) {
            negative_cycle = true;
            break;
        }
    }

    struct {
        decltype(distance) distance;
        decltype(predecessor) predecessor;
        bool negative_cycle;
    } res {std::move(distance), std::move(predecessor), negative_cycle};

    return std::move(res);
}


}