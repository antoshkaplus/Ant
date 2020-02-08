#pragma once

#include <boost/iterator/transform_iterator.hpp>

#include "ant/core/core.hpp"
#include "ant/core/range/flatten.hpp"
#include "ant/core/range/filter.hpp"
#include "ant/core/range/transform.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/edge_no_descriptor.hpp"


namespace ant::graph::model::adj_list {

// just vertices, edges don't have descriptors to access
template<typename Policy, typename VertexInfo, template <typename> class VertexType_, template <typename> class Mutator_>
class Model_NoEdgeDescriptor : public Policy {
    friend class Mutator_<Model_NoEdgeDescriptor>;
    friend class VertexType_<Model_NoEdgeDescriptor>;

    std::vector<VertexInfo> vertices_info;

public:
    using VertexDescriptor = typename VertexInfo::VertexDescriptor;
    using VertexType = VertexType_<Model_NoEdgeDescriptor>;
    using Mutator = Mutator_<Model_NoEdgeDescriptor>;

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
        auto flat_range = FlatRange(range.begin(), range.end(), [](VertexType vertex) {
            return vertex.advance();
        });
        auto to_edge = [](auto advance) {
            return advance.edge();
        };

        if constexpr (is_directed_v<Policy>()) {
            return ant::core::range::TransformRange(std::move(flat_range), std::move(to_edge));
        } else {
            return ant::core::range::TransformRange(
                    ant::core::range::FilterRange(std::move(flat_range),
                    [](auto& advance) { return advance.to().descriptor() > advance().from().descriptor(); }),
                    std::move(to_edge));
        }
    }
};

}