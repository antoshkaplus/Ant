#pragma once

#include <boost/iterator/transform_iterator.hpp>

#include "ant/core/core.hpp"
#include "ant/core/range/range.hpp"
#include "index_vertex_iterator.hpp"
#include "edge_no_descriptor.hpp"
#include "edge_info.hpp"
#include "edge_subscript.hpp"
#include "advance_edges_info.hpp"
#include "mutator_edges_info.hpp"
#include "model_accessor.hpp"


namespace ant::graph::model::adj_vec {

template<typename Policy>
class Model_EdgesInfo : public Policy {

    using VertexInfo = std::conditional_t<is_vertex_value_v<Policy>,
            VertexInfo_Value<Adjacent_EdgeDescriptor<Policy>, typename Policy::VertexValue>,
            VertexInfo_Adjacent<Adjacent_EdgeDescriptor<Policy>>>;
    using EdgeInfo = std::conditional_t<is_edge_value_v<Policy>,
            EdgeInfo_Value<typename Policy::VertexDescriptor, typename Policy::EdgeValue>,
            EdgeInfo_Vertices<typename Policy::VertexDescriptor>>;

    std::vector<VertexInfo> vertices_info;
    std::vector<EdgeInfo> edges_info;

public:
    using VertexDescriptor = typename Policy::VertexDescriptor;
    using VertexType = VertexSubscript<Model_EdgesInfo, AdvanceRange_EdgesInfo<Model_EdgesInfo>>;
    using EdgeDescriptor = typename Policy::EdgeDescriptor;
    using EdgeType = Edge_Subscript<Model_EdgesInfo>;
    using Mutator = Mutator_EdgesInfo<Model_EdgesInfo>;
    using Advance = Advance_EdgesInfo<Model_EdgesInfo>;

    friend Mutator;
    friend VertexType;
    friend EdgeType;
    friend ModelAccessor<Model_EdgesInfo>;

    // can't be typedef of VertexType, because VertexType depends on the model, otherwise Model will become dependent
    // on VertexType for resolution and we will get circluar dependency
    friend Advance_EdgesInfo<Model_EdgesInfo>;
    friend AdvanceIterator_EdgesInfo<Model_EdgesInfo>;
    friend AdvanceRange_EdgesInfo<Model_EdgesInfo>;

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