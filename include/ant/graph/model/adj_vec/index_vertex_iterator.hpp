#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template<typename Graph>
class IndexVertexIterator : public std::iterator<std::forward_iterator_tag, typename Graph::VertexType> {
    using IndexType = typename Graph::VertexDescriptor;
public:
    IndexVertexIterator() {}
    IndexVertexIterator(Graph& graph, IndexType current)
            : current(current), graph(&graph) {}

    auto operator*() const {
        return graph->vertex(current);
    }

    auto operator*() {
        return graph->vertex(current);
    }

    bool operator==(const IndexVertexIterator& it) const {
        return graph == it.graph && current == it.current;
    }

    bool operator!=(const IndexVertexIterator& it) const {
        return graph != it.graph || current != it.current;
    }

    // pred
    IndexVertexIterator& operator++() {
        ++current;
        return *this;
    }

private:
    IndexType current;
    Graph* graph;
};

}