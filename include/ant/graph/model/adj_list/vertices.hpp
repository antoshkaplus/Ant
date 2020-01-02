#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename VertexDescriptor>
class NoEdgeAdvance {
    VertexDescriptor from_;
    VertexDescriptor to_;

public:
    NoEdgeAdvance(VertexDescriptor from, VertexDescriptor to) : from_(from), to_(to) {}

    VertexDescriptor from() { return from_; }
    VertexDescriptor to() { return to_; }
};


template <typename VertexDescriptorType, typename VertexDescriptorForwardIt>
class NextIterator : std::iterator<std::forward_iterator_tag, typename NoEdgeAdvance<VertexDescriptorType>> {

    VertexDescriptorType from;
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

template <typename Graph>
class IndexVertex {

    using IndexType = typename Graph::VertexDescriptorType;

    IndexType descriptor_;
    Graph& graph;

public:

    IndexVertex(Graph& graph, IndexType descriptor) : descriptor_(descriptor), graph(graph) {}

    IndexType descriptor() {
        return descriptor_;
    }

    auto next() {
        return IteratorRange(
                NextIterator(descriptor_, next_[descriptor_].begin()),
                NextIterator(descriptor_, next_[descriptor_].end()));
    }
};


template <typename Graph>
class IndexVertexIterator : std::iterator<std::forward_iterator_tag, typename Graph::VertexType> {
    using IndexType = typename Graph::VertexDescriptorType;
    using VertexType = typename Graph::VertexType;
public:

    IndexVertexIterator(Graph& graph, VertexType current)
        : current(current) graph(graph) {}

    const auto operator*() const {
        return graph.vertex(current);
    }

    auto operator*() {
        return graph.vertex(current);
    }

    bool operator==(const IndexVertexIterator& it) {
        return &graph == &it.graph && current == it.current;
    }
    bool operator!=(const IndexVertexIterator& it) {
        return &graph != &it.graph || current != it.current;
    }

    // pred
    IndexVertexIterator& operator++() {
        ++current;
        return *this;
    }

private:
    IndexType current;
    Graph& graph;
};


template <typename IndexType>
class Vertices {
    std::vector<std::vector<IndexType>> next_;

public:
    using VertexDescriptorType = IndexType;
    using VertexType = IndexVertex<Vertices>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, next_.size()));
    }

    VertexType vertex(VertexDescriptor descriptor) {
        return VertexType(*this, descriptor);
    }
};

}