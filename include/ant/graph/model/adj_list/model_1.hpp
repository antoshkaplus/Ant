#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"

namespace ant::graph::model::adj_list {

// just vertices, edges don't have descriptors to access
template<typename Next, typename VertexType_>
class Model_1 {
    std::vector<Next> next_;

public:
    using VertexDescriptor = typename Next::VertexDescriptor;
    using VertexType = VertexType_;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, next_.size()));
    }

    VertexType vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(*this, vertex_descriptor);
    }
};

}