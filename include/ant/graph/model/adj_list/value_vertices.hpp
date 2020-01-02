#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename ValueType, typename IndexType>
class ValueNext {
    ValueType value;
    std::vector<IndexType> next;
};

template <typename Graph>
class IndexValueVertex : public IndexVertex<Graph> {

    using ValueType = typename Graph::ValueType;

public:
     auto& value() {
         return next_[descriptor_].value;
     }
};


template <typename ValueType, typename IndexType>
class ValueVertices {
    std::vector<ValueNext<ValueType, IndexType>> next_;

public:
    using VertexDescriptorType = IndexType;
    using VertexType = IndexValueVertex<Vertices>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, next_.size()));
    }

    VertexType vertex(VertexDescriptor descriptor) {
        return VertexType(*this, descriptor);
    }
};


// it's clear that VertexNextType can be something special
// have is't own interface

// right now unclear still how to implement builders.


}