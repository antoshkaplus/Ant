#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename VertexDescriptor>
struct EdgeInfo_Vertices {

    std::array<VertexDescriptor, 2> vertices {};

    EdgeInfo_Vertices(VertexDescriptor from, VertexDescriptor to)
        : vertices({from, to}) {}
};


template <typename VertexDescriptor, typename EdgeValue>
struct EdgeInfo_Value : EdgeInfo_Vertices<VertexDescriptor> {

    EdgeValue value;

    EdgeInfo_Value(VertexDescriptor from, VertexDescriptor to, EdgeValue value)
            : EdgeInfo_Vertices<VertexDescriptor>(from, to), value(value) {}
};

}