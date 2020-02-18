#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename Model>
class Advance_NoEdgeDescriptor {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;

private:
    Model& model;
    VertexDescriptor to_;
    VertexDescriptor from_;

public:
    // my require edge descriptor too
    // also graph is needed to provide Vertex
    Advance_NoEdgeDescriptor(Model& model, VertexDescriptor from, VertexDescriptor to)
        : model(model), from_(from), to_(to) {}

    auto from() { return model.vertex(from_); }
    auto to() { return model.vertex(to_); }

    auto edge() { return typename Model::EdgeType(model, from_, to_); }
};

// need the model
template <typename Model>
class AdvanceIterator_NoEdgeDescriptor : public std::iterator<std::forward_iterator_tag, Advance_NoEdgeDescriptor<Model>> {

    using VertexDescriptor = typename Model::VertexDescriptor;
    using VertexDescriptorForwardIt = decltype(std::declval<Model>().vertices_info.front().adjacent.begin());

    Model* model;
    VertexDescriptor from {};
    VertexDescriptorForwardIt to_iterator {};
public:
    AdvanceIterator_NoEdgeDescriptor() = default;
    AdvanceIterator_NoEdgeDescriptor(Model& model, VertexDescriptor from, VertexDescriptorForwardIt to_iterator)
            : model(&model), from(from), to_iterator(to_iterator) {}

    const auto operator*() const {
        return Advance_NoEdgeDescriptor(*model, from, *to_iterator);
    }

    auto operator*() {
        return Advance_NoEdgeDescriptor(*model, from, *to_iterator);
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
class AdvanceRange_NoEdgeDescriptor : public IteratorRange<AdvanceIterator_NoEdgeDescriptor<Model>, AdvanceIterator_NoEdgeDescriptor<Model>> {
public:
    using Advance = Advance_NoEdgeDescriptor<Model>;
    using Iterator = AdvanceIterator_NoEdgeDescriptor<Model>;

    AdvanceRange_NoEdgeDescriptor(Model& model, typename Model::VertexDescriptor from) :
            IteratorRange<AdvanceIterator_NoEdgeDescriptor<Model>, AdvanceIterator_NoEdgeDescriptor<Model>>(
                AdvanceIterator_NoEdgeDescriptor(model, from, model.vertices_info[from].adjacent.begin()),
                AdvanceIterator_NoEdgeDescriptor(model, from, model.vertices_info[from].adjacent.end())) {}
};


}