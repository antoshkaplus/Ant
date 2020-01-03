#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/advance_1.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model>
class Vertex_1 {
public:
    using VertexDescriptor = typename Model::VertexDescriptor

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    Vertex_1(Model& model, VertexDescriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() { return descriptor_; }         // required

    auto next() {
        return IteratorRange(
                NextIterator_1(descriptor_, model.next_[descriptor_].vertices.begin())
                )
    }

};


}