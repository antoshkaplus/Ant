#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

// also provides edge descriptor
template<typename Policy, typename VertexInfo, template <typename> class VertexType_, template <typename> class Mutator_>
class Model_1_1 {
    friend class Mutator_<Model_1_1>;
    friend class VertexType_<Model_1_1>;

    std::vector<VertexInfo> adjacent_;

public:
    using VertexDescriptor = typename VertexInfo::VertexDescriptor;
    using VertexType = VertexType_<Model_1_1>;
    using Mutator = Mutator_<Model_1_1>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, adjacent_.size()));
    }

    VertexType vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(*this, vertex_descriptor);
    }

    auto edges() {

    }

    auto edge(EdgeDescriptor edge_descriptor) {
        return;
    }
};

}