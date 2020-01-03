#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename VertexDescriptor>
class Advance_1 {
    VertexDescriptor from_;
    VertexDescriptor to_;

public:
    // my require edge descriptor too
    // also graph is needed to provide Vertex
    Advance_1(VertexDescriptor from, VertexDescriptor to) : from_(from), to_(to) {}

    VertexDescriptor from() { return from_; }
    VertexDescriptor to() { return to_; }
};

template <typename VertexDescriptor, typename VertexDescriptorForwardIt>
class NextIterator_1 : std::iterator<std::forward_iterator_tag, typename Advance_1<VertexDescriptor>> {

    VertexDescriptor from;
    VertexDescriptorForwardIt to_iterator;
public:
    NextIterator(VertexDescriptorType from, VertexDescriptorForwardIt to_iterator)
            : from(from), to_iterator(to_iterator) {}

    const NoEdgeAdvance operator*() const {
        return NoEdgeAdvance(from, *to_iterator);
    }

    auto operator*() {
        return NoEdgeAdvance(from, *to_iterator);
    }

    bool operator==(const NextIterator& it) {
        return from == it.from && to_iterator == it.to_iterator;
    }
    bool operator!=(const NextIterator& it) {
        return from != it.from || to_iterator != it.to_iterator;
    }

    // pred
    NextIterator& operator++() {
        ++to_iterator;
    }
};


}