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
class AdvanceIterator_1 : std::iterator<std::forward_iterator_tag, Advance_1<VertexDescriptor>> {

    VertexDescriptor from {};
    VertexDescriptorForwardIt to_iterator {};
public:
    AdvanceIterator_1() {}
    AdvanceIterator_1(VertexDescriptor from, VertexDescriptorForwardIt to_iterator)
            : from(from), to_iterator(to_iterator) {}

    const auto operator*() const {
        return Advance_1(from, *to_iterator);
    }

    auto operator*() {
        return Advance_1(from, *to_iterator);
    }

    bool operator==(const AdvanceIterator_1& it) const {
        return from == it.from && to_iterator == it.to_iterator;
    }
    bool operator!=(const AdvanceIterator_1& it) const {
        return from != it.from || to_iterator != it.to_iterator;
    }

    // pred
    AdvanceIterator_1& operator++() {
        ++to_iterator;
        return *this;
    }
};


}