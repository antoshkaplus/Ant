#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"

namespace ant::graph::model::adj_list {

// just vertices, edges don't have descriptors to access
template<typename Policy, typename VertexInfo, template <typename> class VertexType_, template <typename> class Mutator_>
class Model_NoEdgeDescriptor : public Policy {
    friend class Mutator_<Model_NoEdgeDescriptor>;
    friend class VertexType_<Model_NoEdgeDescriptor>;

    std::vector<VertexInfo> vertices_info;

public:
    using VertexDescriptor = typename VertexInfo::VertexDescriptor;
    using VertexType = VertexType_<Model_NoEdgeDescriptor>;
    using Mutator = Mutator_<Model_NoEdgeDescriptor>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, vertices_info.size()));
    }

    VertexType vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(*this, vertex_descriptor);
    }

    auto edges() {

    }
};

}