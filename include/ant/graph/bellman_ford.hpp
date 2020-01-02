#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/graph.hpp"

namespace ant::graph {

template <typename Graph, typename = std::enable_if_t<
        is_directed_v<Graph> &&
        is_vertex_id_index_v<Graph> &&
        std::is_signed_v<Graph::VertexIdType> &&
        std::is_arithmetic_v<typename Graph::EdgeValueType>>>
void BellmanFord(const Graph& G, typename Graph::VertixIdType source) {

    std::vector<typename Graph::EdgeValueType> distance(G.vertex_count(), std::numeric_limits<typename Graph::EdgeValueType>::max());
    std::vector<typename Graph::VertixIdType> predecessor(G.vertex_count(), -1);

    distance[source] = typename Graph::EdgeValueType {};

    for (typename Graph::VertixIdType i = 0; i < G.vertex_count()-1; ++i) {
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