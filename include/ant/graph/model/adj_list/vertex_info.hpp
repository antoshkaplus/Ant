#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename Policy>
struct Adjacent_EdgeDescriptor {
    typename Policy::VertexDescriptor vertex_descriptor;
    typename Policy::EdgeDescriptor edge_descriptor;
};


template <typename Adjacent>
struct VertexInfo_Adjacent {
    std::vector<Adjacent> adjacent;
};

template <typename Policy, typename Adjacent>
struct VertexInfo_Value : VertexInfo_Adjacent<Adjacent> {
    typename Policy::VertexValue value;
};

}