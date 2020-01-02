#pragma once

#include "ant/graph/model/adj_list/vertices_mutator.hpp"

namespace ant::graph::model::adj_list {

template <typename Graph>
class VerticesMutator {

    VerticesMutator(Graph& graph) {}

    // we don't say IteratorType
    VertexDescriptor AddVertex() {
        // ...
    }

    void RemoveVertex(VertexDescriptor) {
        if (not last) throw std::runtime_error("not supported");
    }

    void AddEdge() {
        // not needed for now
    }
};

}