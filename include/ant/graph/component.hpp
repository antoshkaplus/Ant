#pragma once

#include "ant/graph/graph.hpp"

namespace ant::graph {

// it's your responsibility to pass me a stupid graph, instead of smart one
// maybe use type that's already created
template<class Graph, typename std::enable_if<Graph::directed==true>::type* = nullptr>
std::vector<typename Graph::NodeType> Kosaraju(Graph& graph) {
    using NodeType = typename Graph::NodeType;
    std::vector<NodeType> L;
    std::vector<bool> visited(graph.nodeCount(), false);

    std::function<void(NodeType)> Visit = [&](NodeType u) -> void {
        if (!visited[u]) {
            visited[u] = true;
            // out-neighbour
            for (auto v : graph.nextNodes(u)) {
                Visit(v);
            }
            L.push_back(u);
        }
    };

    for (auto u = 0; u < graph.nodeCount(); ++u) {
        Visit(u);
    }

    auto reversedGraph = Reverse(graph);

    std::reverse(L.begin(), L.end());
    // assigned to component
    std::fill(visited.begin(), visited.end(), false);
    std::vector<typename Graph::NodeType> components;

    std::function<void(NodeType,NodeType)> Assign = [&](NodeType u, NodeType root) -> void {
    //auto Assign = [&](auto u, auto root) -> void {
        if (!visited[u]) {
            components[u] = root;
            // in-neighbour
            for (auto v : reversedGraph.nextNodes(u)) {
                Assign(v, root);
            }
        }
    };

    for (auto u : L) {
        Assign(u, u);
    }
    return components;
}

}

