#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"

namespace ant::graph::model::adj_vec {

template <typename Model>
class Edge_NoDescriptor {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    using VertexType = typename Model::VertexType;

private:
    Model& model;
    VertexDescriptor from_;
    VertexDescriptor to_;

public:
    Edge_NoDescriptor(Model& model, VertexDescriptor from, VertexDescriptor to)
        : model(model), from_(from), to_(to) {}

    template <typename V = VertexType>
    ResultEnableIf<is_directed_v<Model>, V> from() {
        return model.vertex(from_);
    }

    template <typename V = VertexType>
    ResultEnableIf<is_directed_v<Model>, V> to() {
        return model.vertex(to_);
    }

    template <typename V = VertexType>
    ResultEnableIf<!is_directed_v<Model>, std::array<V, 2>> vertices() {
        return {model.vertex(from_), model.vertex(to_)};
    }
};

}