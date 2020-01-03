#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template<typename Graph>
class IndexVertexIterator : std::iterator<std::forward_iterator_tag, typename Graph::VertexType> {
    using IndexType = typename Graph::VertexDescriptorType;
    using VertexType = typename Graph::VertexType;
public:

    IndexVertexIterator(Graph& graph, VertexType current)
            : current(current), graph(graph) {}

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

}