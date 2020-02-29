#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"
#include "index_vertex_iterator.hpp"

namespace ant::graph::model::adj_vec {

template<typename Model>
class Mutator_NoEdgeDescriptor {

    Model& model;

public:
    using VertexDescriptor = typename Model::VertexDescriptor;

    Mutator_NoEdgeDescriptor(Model& model) : model(model) {}

    VertexDescriptor AddVertex() {
        auto v = model.vertices_info.size();
        model.vertices_info.resize(v + 1);
        return static_cast<VertexDescriptor>(v);
    }

    void AddEdge(VertexDescriptor from, VertexDescriptor to) {
        model.vertices_info[from].adjacent.push_back(to);
        if constexpr (!is_directed_v<Model>) {
            model.vertices_info[to].adjacent.push_back(from);
        }
    }

    void SetVertexCount(VertexDescriptor count) {
        model.vertices_info.resize(count);
    }
};

}