#pragma once

#include "ant/graph/model/graph_3.hpp"

using namespace std;

namespace ant::graph::test {

struct Graph_EdgeWeights {
    EdgedGraph<int, int> graph;
    vector<Int> edge_weights;
};


// completeness between 0 and 1 : ratio of edges
inline Graph_EdgeWeights RandomDirEdgedGraph(Count node_count, double completeness, Count max_edge_value) {

    std::default_random_engine rng;
    std::uniform_real_distribution<> distr;
    std::uniform_int_distribution<> val_distr(1, max_edge_value);
    DirEdgedGraphBuilder<int, int> builder(node_count);
    vector<int> edgeVals;
    for (auto i = 0; i < node_count; ++i) {
        for (auto j = 0; j < node_count; ++j) {
            if (i == j || distr(rng) > completeness) continue;
            builder.add(i, j);
            edgeVals.push_back(val_distr(rng));
        }
    }

    return {builder.build(), edgeVals};
}

inline Graph_EdgeWeights RandomUndirEdgedGraph(Count node_count, double completeness, Count max_edge_value) {

    std::default_random_engine rng;
    std::uniform_real_distribution<> distr;
    std::uniform_int_distribution<> val_distr(1, max_edge_value);
    UndirEdgedGraphBuilder<int, int> builder(node_count);
    vector<int> edgeVals;
    for (auto i = 0; i < node_count; ++i) {
        for (auto j = 0; j < node_count; ++j) {
            if (i == j || distr(rng) > completeness) continue;
            builder.add(i, j);
            edgeVals.push_back(val_distr(rng));
        }
    }

    return {builder.build(), edgeVals};
}

template <typename RNG>
Graph_EdgeWeights RandomConnectedUndirEdgedGraph(
    Count node_count, Count edge_count, Count max_edge_value, RNG& rng) {

    if (node_count > edge_count+1) throw std::runtime_error("too few edges to make connected graph");

    std::uniform_int_distribution<> val_distr(1, max_edge_value);
    UndirEdgedGraphBuilder<int, int> builder(node_count);

    vector<Index> nodes(node_count);
    std::iota(nodes.begin(), nodes.end(), 0);
    std::shuffle(nodes.begin(), nodes.end(), rng);

    vector<int> edgeVals;
    // each time introduce new node
    for (auto i = 1; i < node_count; ++i) {
        std::uniform_int_distribution<> new_node_distr(0, i-1);
        builder.add(nodes[new_node_distr(rng)], nodes[i]);
        edgeVals.push_back(val_distr(rng));
    }

    std::uniform_int_distribution<> node_distr(0, node_count-1);
    for (auto j = node_count-1; j < edge_count; ++j) {
        builder.add(node_distr(rng), node_distr(rng));
        edgeVals.push_back(val_distr(rng));

    }

    return {builder.build(), edgeVals};
}



}