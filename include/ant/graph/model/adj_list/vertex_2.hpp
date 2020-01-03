#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model>
class Vertex_2 {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    using VertexValue = typename Model::VertexValue;

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    Vertex_1(Model& model, VertexDescriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() { return descriptor_; }         // required
    VertexValue& value() {
        return model.adjacent_[descriptor_].value;
    }
};


}