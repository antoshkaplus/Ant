#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename Model>
class Advance_EdgesInfo {
public:
    using EdgeInfo = typename Model::EdgeInfo;

private:
    Model& model;
    VertexDescriptor to_;
    VertexDescriptor edge_;

public:
    // my require edge descriptor too
    // also graph is needed to provide Vertex
    Advance_EdgesInfo(Model& model, VertexDescriptor to, EdgeDescriptor edge)
            : model(model), to_(to), edge_(edge) {}

    auto to() { return model.vertex(to_); }

    auto edge() { return model.edge(edge_); }
};

// need the model
template <typename Model>
class AdvanceIterator_EdgesInfo : public std::iterator<std::forward_iterator_tag, Advance_NoEdgeDescriptor<Model>> {

    using VertexDescriptor = typename Model::VertexDescriptor;
    using VertexDescriptorForwardIt = decltype(std::declval<Model>().vertices_info.begin());

    Model* model;
    VertexDescriptor from {};
    VertexDescriptorForwardIt to_iterator {};
public:
    AdvanceIterator_EdgesInfo() = default;
    AdvanceIterator_EdgesInfo(Model& model, VertexDescriptor from, VertexDescriptorForwardIt to_iterator)
            : model(&model), from(from), to_iterator(to_iterator) {}

    const auto operator*() const {
        return Advance_EdgesInfo(*model, to_iterator->vertex_descriptor, to_iterator->edge_descriptor);
    }

    auto operator*() {
        return Advance_EdgesInfo(*model, to_iterator->vertex_descriptor, to_iterator->edge_descriptor);
    }

    bool operator==(const AdvanceIterator_NoEdgeDescriptor& it) const {
        return from == it.from && to_iterator == it.to_iterator;
    }
    bool operator!=(const AdvanceIterator_NoEdgeDescriptor& it) const {
        return from != it.from || to_iterator != it.to_iterator;
    }

    // pred
    AdvanceIterator_NoEdgeDescriptor& operator++() {
        ++to_iterator;
        return *this;
    }
};

template <typename Model>
class AdvanceRange_EdgesInfo : public IteratorRange<AdvanceIterator_EdgesInfo<Model>, AdvanceIterator_EdgesInfo<Model>> {
public:
    AdvanceRange_EdgesInfo(Model& model, typename Model::VertexDescriptor from) :
            IteratorRange<AdvanceIterator_EdgesInfo<Model>, AdvanceIterator_EdgesInfo<Model>>(
                AdvanceIterator_EdgesInfo(model, from, model.vertices_info[from].adjacent.begin()),
                AdvanceIterator_EdgesInfo(model, from, model.vertices_info[from].adjacent.end())) {}
};


}