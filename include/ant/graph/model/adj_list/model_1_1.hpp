#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

// also provides edge descriptor
template<typename Adjacent>
class Model_1_1 {
    std::vector<Adjacent> adjacent_;

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
        return
    }
};

}