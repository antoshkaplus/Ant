#pragma once


#include "ant/graph/graph.hpp"


using namespace std;

namespace ant::graph::test {


// completeness between 0 and 1 : ratio of edges
template <typename Policy, std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> && is_edge_value_v<Policy>, int> = 0>
inline Graph<Policy> RandomEdgeValueGraph(Count node_count, double completeness, Count max_edge_value) {

    std::default_random_engine rng {};
    std::uniform_real_distribution<> distr;
    std::uniform_int_distribution<> val_distr(1, max_edge_value);

    Graph<Policy> graph {};
    typename Graph<Policy>::Mutator mutator(graph);
    mutator.SetVertexCount(node_count);

    for (auto i = 0; i < node_count; ++i) {
        auto j_bound = node_count;
        if constexpr (!is_directed_v<Policy>) {
            j_bound /= 2;
        }
        for (auto j = 0; j < node_count; ++j) {
            if (i == j || distr(rng) > completeness) continue;
            mutator.AddEdge(i, j, val_distr(rng));
        }
    }

    return graph;
}

template <typename Policy, typename RNG, std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> && is_edge_value_v<Policy> && !is_directed_v<Policy>, int> = 0>
inline Graph<Policy> RandomConnectedEdgedValueGraph(
    Count node_count, Count edge_count, Count max_edge_value, RNG& rng) {

    if (node_count > edge_count+1) throw std::runtime_error("too few edges to make connected graph");

    std::uniform_int_distribution<> val_distr(1, max_edge_value);

    Graph<Policy> graph {};
    typename Graph<Policy>::Mutator mutator(graph);
    mutator.SetVertexCount(node_count);

    vector<Index> nodes(node_count);
    std::iota(nodes.begin(), nodes.end(), 0);
    std::shuffle(nodes.begin(), nodes.end(), rng);

    vector<int> edgeVals;
    // each time introduce new node
    for (auto i = 1; i < node_count; ++i) {
        std::uniform_int_distribution<> new_node_distr(0, i-1);
        mutator.AddEdge(nodes[new_node_distr(rng)], nodes[i], val_distr(rng));
    }

    std::uniform_int_distribution<> node_distr(0, node_count-1);
    for (auto j = node_count-1; j < edge_count; ++j) {
        mutator.AddEdge(node_distr(rng), node_distr(rng), val_distr(rng));
    }

    return graph;
}



}