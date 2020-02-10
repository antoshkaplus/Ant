#pragma once

#include <boost/iterator/transform_iterator.hpp>

#include "ant/core/core.hpp"
#include "ant/core/range/flatten.hpp"
#include "ant/core/range/filter.hpp"
#include "ant/core/range/transform.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/edge_no_descriptor.hpp"
#include "ant/graph/model/adj_list/edge_info.hpp"
#include "ant/graph/model/adj_list/edge_subscript.hpp"


namespace ant::graph::model::adj_list {

// just vertices, edges don't have descriptors to access
template<typename Policy, typename VertexInfo, typename EdgeInfo, class VertexType_, template <typename> class Mutator_>
class Model_EdgesInfo : public Policy {
    friend class Mutator_<Model_EdgesInfo>;
    friend VertexType_;

    std::vector<VertexInfo> vertices_info;
    std::vector<EdgeInfo> edges_info;

public:
    using VertexDescriptor = typename Policy::VertexDescriptor;
    using VertexType = VertexType_;
    using EdgeDescriptor = typename Policy::EdgeDescriptor;
    using EdgeType = Edge_Subscript<Model_EdgesInfo>;
    using Mutator = Mutator_<Model_EdgesInfo>;

    auto vertices() {
        return IteratorRange(
                IndexVertexIterator(*this, 0),
                IndexVertexIterator(*this, vertices_info.size()));
    }

    VertexType vertex(VertexDescriptor vertex_descriptor) {
        return VertexType(*this, vertex_descriptor);
    }

    auto edges() {
        auto range = vertices();
        auto to_edge = [](auto advance) {
            return advance.edge();
        };

        if constexpr (is_directed_v<Policy>) {
            auto flat_range = FlatRange(range.begin(), range.end(), [](VertexType vertex) {
                return vertex.advance();
            });
            return ant::core::range::TransformRange(std::move(flat_range), std::move(to_edge));
        } else {
            auto flat_range = FlatRange(range.begin(), range.end(), [](VertexType vertex) {
                return ant::core::range::FilterRange(vertex.advance(), std::function<bool(typename VertexType::Advance&)>([from=vertex.descriptor()](auto& advance) -> bool {
                    return advance.to().descriptor() > from;
                }));
            });
            return ant::core::range::TransformRange(std::move(flat_range), std::move(to_edge));
        }
    }

    EdgeType edge(EdgeDescriptor edge_descriptor) {
        return EdgeType(*this, edge_descriptor);
    }
};

}