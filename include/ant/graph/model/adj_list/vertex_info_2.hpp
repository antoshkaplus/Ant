#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

template <typename VertexIndex, typename EdgeDescriptor, typename EdgeValue>
class VertexInfo_2 {

public:
    using VertexDescriptor = VertexIndex;

    struct Adjacent {
        VertexDescriptor;
        EdgeDescriptor;
        EdgeValue;
    };

    std::vector<Adjacent> adjacent;
};

}