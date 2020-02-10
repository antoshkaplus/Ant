#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"
#include "ant/graph/model/adj_list/model_no_edge_descriptor.hpp"
#include "ant/graph/model/adj_list/model_edges_info.hpp"
#include "ant/graph/model/adj_list/vertex_info.hpp"
#include "ant/graph/model/adj_list/edge_info.hpp"
#include "ant/graph/model/adj_list/vertex_subscript.hpp"
#include "ant/graph/model/adj_list/mutator_no_edge_descriptor.hpp"

namespace ant::graph::model::adj_list {

template <typename Policy, typename Enable = void>
class Graph;

template <typename Policy>
class Graph<Policy,

        std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> &&
        !is_vertex_value_v<Policy> &&
        !is_edge_descriptor_v<Policy>

        >> : public Model_NoEdgeDescriptor<Policy, VertexInfo_Adjacent<typename Policy::VertexDescriptor>,
        VertexSubscript<Graph<Policy>, AdvanceRange_NoEdgeDescriptor<Graph<Policy>>>,
        Mutator_NoEdgeDescriptor> {
public:
};

template <typename Policy>
class Graph<Policy,

        std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> &&
        is_vertex_value_v<Policy> &&
        !is_edge_descriptor_v<Policy>

        >> : public Model_NoEdgeDescriptor<Policy, VertexInfo_Value<typename Policy::VertexDescriptor, typename Policy::VertexValue>,
        VertexSubscript<Graph<Policy>, AdvanceRange_NoEdgeDescriptor<Graph<Policy>>>, Mutator_NoEdgeDescriptor> {
public:
};

template <typename Policy>
class Graph<Policy,

        std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> &&
        !is_vertex_value_v<Policy> &&
        is_edge_descriptor_index_v<Policy>

        >> : public Model_EdgesInfo<Policy, VertexInfo_Adjacent<Adjacent_EdgeDescriptor<Policy>>, EdgeInfo_Vertices<typename Policy::VertexDescriptor>,
        VertexSubscript<Graph<Policy>, AdvanceRange_NoEdgeDescriptor<Graph<Policy>>>, Mutator_NoEdgeDescriptor> {
public:
};

}
