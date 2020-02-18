#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"
#include "index_vertex_iterator.hpp"

namespace ant::graph::model::adj_list {

template<typename Model>
class Mutator_EdgesInfo {

    Model& model;

public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    using EdgeDescriptor = typename Model::EdgeDescriptor;

    Mutator_EdgesInfo(Model& model) : model(model) {}

    VertexDescriptor AddVertex() {
        auto v = model.vertices_info.size();
        model.vertices_info.resize(v + 1);
        return static_cast<VertexDescriptor>(v);
    }

    EdgeDescriptor AddEdge(VertexDescriptor from, VertexDescriptor to) {
        auto new_edge = model.edges_info.size();

        if constexpr (is_edge_value_v<Model>) {
            model.edges_info.emplace_back(from, to, typename Model::EdgeValue{});
        } else {
            model.edges_info.emplace_back(from, to);
        }
        model.vertices_info[from].adjacent.emplace_back(to, new_edge);
        if constexpr (!is_directed_v<Model>) {
            model.vertices_info[to].adjacent.emplace_back(from, new_edge);
        }
        return new_edge;
    }

    void SetVertexCount(VertexDescriptor count) {
        model.vertices_info.resize(count);
    }
};

}