#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

// based on Model_1_1 but edges related info kept separate from Adjacency List
template<typename Adjacent, typename Edge>
class Model_2 {
    std::vector<Adjacent> adjacent_;
    std::vector<Edge> edges_;

public:
    using VertexDescriptor = typename Adjacent::VertexDescriptor;
    using Vertex = IndexVertex<Vertices>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, adjacent_.size()));
    }

    Vertex vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(*this, vertex_descriptor);
    }

    Edge edge(EdgeDescriptor edge_descriptor) {

    }
};

}