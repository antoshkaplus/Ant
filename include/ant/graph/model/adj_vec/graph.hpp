#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"

#include "model_no_edge_descriptor.hpp"
#include "model_edges_info.hpp"
#include "vertex_info.hpp"
#include "edge_info.hpp"
#include "vertex_subscript.hpp"
#include "mutator_no_edge_descriptor.hpp"

namespace ant::graph::model::adj_vec {

template <typename Policy, typename Enable = void>
class Graph;

// Because at Graph definition point this is not yet complete type,
// can't declare support types. In that case all of it has to go into InternalPolicy class somehow,

// another soluton would be to decide types inside the Model itself. Probably the best solution.
// At that point could be seen as that flexible

// Can't see why not either

template <typename Policy>
class Graph<Policy,

        std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> &&
        !is_edge_descriptor_v<Policy>

        >> : public Model_NoEdgeDescriptor<Policy> {
public:
};

template <typename Policy>
class Graph<Policy,

        std::enable_if_t<
        is_vertex_descriptor_index_v<Policy> &&
        is_edge_descriptor_index_v<Policy>

        >> : public Model_EdgesInfo<Policy> {
public:
};

// TODO think more on this one
template <typename Policy>
ant::Count CountVertices(Model_NoEdgeDescriptor<Policy>& g) {
    return ModelAccessor(g).vertices_info().size();
}
template <typename Policy>
ant::Count CountVertices(Model_EdgesInfo<Policy>& g) {
    return ModelAccessor(g).vertices_info().size();
}

}
