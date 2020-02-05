#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/advance_1.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model>
class VertexSub {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    // TODO resolve this
    using VertexValue = typename Model::VertexValue;

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    VertexSub(Model& model, VertexDescriptor descriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() const { return descriptor_; }

    auto advance() {
        return IteratorRange(
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.begin()),
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.end()));
    }

    template <typename V = VertexValue>
    ResultEnableIf<!std::is_same_v<void, V>, V&> value() {
        return model.vertices_info[descriptor_].value;
    }
};


}