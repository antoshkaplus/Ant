#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/advance_1.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model>
class Vertex_1 {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    Vertex_1(Model& model, VertexDescriptor descriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() { return descriptor_; }         // required

    auto advance() {
        return IteratorRange(
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.begin()),
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.end()));
    }

};


}