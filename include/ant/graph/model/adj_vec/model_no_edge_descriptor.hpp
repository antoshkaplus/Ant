#pragma once

#include <boost/iterator/transform_iterator.hpp>

#include "ant/core/core.hpp"
#include "ant/core/range/flatten.hpp"
#include "ant/core/range/filter.hpp"
#include "ant/core/range/transform.hpp"
#include "index_vertex_iterator.hpp"
#include "edge_no_descriptor.hpp"
#include "vertex_info.hpp"
#include "vertex_subscript.hpp"
#include "mutator_no_edge_descriptor.hpp"


namespace ant::graph::model::adj_list {

// just vertices, edges don't have descriptors to access
template<typename Policy>
class Model_NoEdgeDescriptor : public Policy {

    using VertexInfo = std::conditional_t<is_vertex_value_v<Policy>,
            VertexInfo_Value<typename Policy::VertexDescriptor, typename Policy::VertexValue>,
            VertexInfo_Adjacent<typename Policy::VertexDescriptor>>;

    std::vector<VertexInfo> vertices_info;

public:
    using VertexDescriptor = typename Policy::VertexDescriptor;
    using VertexType = VertexSubscript<Model_NoEdgeDescriptor, AdvanceRange_NoEdgeDescriptor<Model_NoEdgeDescriptor>>;
    using EdgeType = Edge_NoDescriptor<Model_NoEdgeDescriptor>;
    using Mutator = Mutator_NoEdgeDescriptor<Model_NoEdgeDescriptor>;
    using Advance = Advance_NoEdgeDescriptor<Model_NoEdgeDescriptor>;

    friend Mutator;
    friend VertexType;
    friend Advance_NoEdgeDescriptor<Model_NoEdgeDescriptor>;
    friend AdvanceIterator_NoEdgeDescriptor<Model_NoEdgeDescriptor>;
    friend AdvanceRange_NoEdgeDescriptor<Model_NoEdgeDescriptor>;

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
};

}