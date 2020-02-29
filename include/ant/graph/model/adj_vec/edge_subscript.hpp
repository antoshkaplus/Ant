#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"

namespace ant::graph::model::adj_vec {

template <typename Model>
class Edge_Subscript {
public:
    using EdgeDescriptor = typename Model::EdgeDescriptor;
    using VertexType = typename Model::VertexType;
    using EdgeValue = typename Model::EdgeValue;

private:
    Model& model;
    EdgeDescriptor descriptor_;

public:
    Edge_Subscript(Model& model, EdgeDescriptor descriptor)
            : model(model), descriptor_(descriptor) {}

    EdgeDescriptor descriptor() {
        return descriptor_;
    }

    template <typename V = VertexType>
    ResultEnableIf<is_directed_v<Model>, V> from() {
        return model.vertex(model.edges_info[descriptor_].vertices[0]);
    }

    template <typename V = VertexType>
    ResultEnableIf<is_directed_v<Model>, V> to() {
        return model.vertex(model.edges_info[descriptor_].vertices[1]);
    }

    template <typename V = VertexType>
    ResultEnableIf<!is_directed_v<Model>, std::array<V, 2>> vertices() {
        return { VertexType(model, model.edges_info[descriptor_].vertices[0]) ,
                 VertexType(model, model.edges_info[descriptor_].vertices[1])};
    }

    template <typename V = EdgeValue>
    ResultEnableIf<!std::is_void_v<V>, V&> value() {
        return model.edges_info[descriptor_].value;
    }
};

}