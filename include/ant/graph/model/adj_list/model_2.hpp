#pragma once

#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

// also provides edge descriptor

// once using iterators for edges and vertices, don't need to pass more stuff

template <typename VertexValue>
struct VertexInfo {
    // maybe another iterator for next vertex, faster navigation
    std::list<std::list<EdgeInfo>::iterator> adjacent_;
    VertexValue value_;
};

template <typename EdgeValue>
struct EdgeInfo {
    std::list<EdgeInfo>::iterator descriptor;

    std::list<VertexInfo>::iterator source;
    std::list<VertexInfo>::iterator target;

    std::array<std::list<VertexInfo>::iterator, 2> vertices;

    EdgeValue value_;
};


template<typename Policy, typename VertexInfo, template <typename> class VertexType_, template <typename> class Mutator_>
class Model_EdgesInfo {
    friend class Mutator_<Model_EdgesInfo>;
    friend class VertexType_<Model_EdgesInfo>;

    std::list<VertexInfo> vertices_info_;
    std::list<EdgeInfo> edges_info_;

public:
    using VertexDescriptor = std::list<VertexInfo>::iterator;
    using EdgeDescriptor = std::list
    using VertexType = VertexType_<Model_EdgesInfo>;
    using Mutator = Mutator_<Model_EdgesInfo>;

    auto vertices() {
        return IteratorRange(adjacent_.begin(), adjacent_.end());
    }

    auto vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(vertex_descriptor);
    }

    auto edges() {
        return IteratorRange(edges_.begin(), edges_.end());
    }

    auto edge(EdgeDescriptor edge_descriptor) {
        return EdgeType(edge_descriptor);
    }
};

}