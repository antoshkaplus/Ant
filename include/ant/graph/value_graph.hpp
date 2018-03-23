#pragma once

#include "ant/core/core.hpp"


namespace ant::graph {

template<class Graph, class Value>
struct ValueNodes {
    Graph& graph;
    std::vector<Value>& values;
};

template<class Graph, class Value>
struct ValueEdges {
    Graph& graph;
    std::vector<Value>& values;
};

template<class Graph, class NodeValue, class EdgeValue>
struct ValueGraph {
    Graph& graph;
    std::vector<NodeValue> nodeValues;
    std::vector<EdgeValue> edgeValues;
};

}