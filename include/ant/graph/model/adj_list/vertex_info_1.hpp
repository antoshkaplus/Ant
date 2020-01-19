#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename VertexIndex>
class VertexInfo_1 {
public:
    using VertexDescriptor = VertexIndex;

    std::vector<VertexDescriptor> adjacent;
};

}