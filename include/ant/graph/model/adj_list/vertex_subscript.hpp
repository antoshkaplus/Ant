#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/advance_no_edge_descriptor.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model, typename AdvanceRange>
class VertexSubscript {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    // TODO resolve this
    using VertexValue = typename Model::VertexValue;
    using Advance = Advance_NoEdgeDescriptor<Model>;

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    VertexSubscript(Model& model, VertexDescriptor descriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() const { return descriptor_; }

    auto advance() {
        return AdvanceRange(model, descriptor_);
    }

    template <typename V = VertexValue>
    ResultEnableIf<!std::is_void_v<V>, V&> value() {
        return model.vertices_info[descriptor_].value;
    }
};


}