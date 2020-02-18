#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename Policy>
struct Adjacent_EdgeDescriptor {

    typename Policy::VertexDescriptor vertex_descriptor;
    typename Policy::EdgeDescriptor edge_descriptor;

    Adjacent_EdgeDescriptor() = default;
    Adjacent_EdgeDescriptor(typename Policy::VertexDescriptor vertex_descriptor, typename Policy::EdgeDescriptor edge_descriptor)
        : vertex_descriptor(vertex_descriptor), edge_descriptor(edge_descriptor) {}
};


template <typename Adjacent>
struct VertexInfo_Adjacent {
    std::vector<Adjacent> adjacent;
};

template <typename Adjacent, typename VertexValue>
struct VertexInfo_Value : VertexInfo_Adjacent<Adjacent> {
    VertexValue value;
};

}