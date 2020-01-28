#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"
#include "ant/graph/model/adj_list/model_no_edge_descriptor.hpp"
#include "ant/graph/model/adj_list/vertex_info_1.hpp"
#include "ant/graph/model/adj_list/vertex_descriptor.hpp"
#include "ant/graph/model/adj_list/mutator_1.hpp"

namespace ant::graph::model::adj_list {

template <typename Policy,
    std::enable_if_t<
    is_vertex_descriptor_index_v<Policy> &&
    !is_vertex_value_v<Policy> &&
    !is_edge_descriptor_v<Policy>
    , int> = 0>
class Graph : public Model_NoEdgeDescriptor<Policy, VertexInfo_1<typename Policy::VertexDescriptor>, Vertex_1, Mutator_1> {
public:
};



};
