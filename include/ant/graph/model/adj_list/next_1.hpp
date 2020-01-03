#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

    typename <typename VertexIndex>
    class Next_1 {
        using VertexDescriptor = VertexIndex;

        std::vector<VertexDescriptor> adjacent;
    };

}