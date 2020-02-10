#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"

namespace ant::graph::model::adj_list {

template <typename Model>
class Edge_Subscript {
public:
    using EdgeDescriptor = typename Model::EdgeDescriptor;
    using VertexType = typename Model::VertexType;

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
        return model.edges_info[descriptor_];
    }

    template <typename V = VertexType>
    ResultEnableIf<is_directed_v<Model>, V> to() {
        return model.edges_info[descriptor_];
    }

    template <typename V = VertexType>
    ResultEnableIf<!is_directed_v<Model>, std::array<V, 2>> vertices() {
        return model.edges_info[descriptor_];
    }
};

}