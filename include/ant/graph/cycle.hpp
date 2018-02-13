#pragma once

#include "ant/graph/component.hpp"

namespace ant::graph {


template<class Graph, typename std::enable_if<Graph::directed==true>::type* = nullptr>
bool IsCyclic(const Graph& g) {

    auto components = Kosaraju(g);
    std::vector<bool> visited(g.nodeCount());
    for (auto c : components) {
        if (visited[c]) {
            return true;
        }
        visited[c] = true;
    }
    return false;

}

}