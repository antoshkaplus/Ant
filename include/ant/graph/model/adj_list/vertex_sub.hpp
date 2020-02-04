#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/model/adj_list/index_vertex_iterator.hpp"
#include "ant/graph/model/adj_list/advance_1.hpp"

namespace ant::graph::model::adj_list {

// vertex gets returned by
template <typename Model>
class VertexSub {
public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    // TODO resolve this
    using VertexValue = typename Model::VertexValue;

private:
    VertexDescriptor descriptor_;
    Model& model;

public:
    VertexSub(Model& model, VertexDescriptor descriptor) : model(model), descriptor_(descriptor) {}

    VertexDescriptor descriptor() const { return descriptor_; }

    auto advance() {
        return IteratorRange(
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.begin()),
                AdvanceIterator_1(descriptor_, model.vertices_info[descriptor_].adjacent.end()));
    }

    // TODO need to enable if in return type

//    template<class Q = T>
//    typename std::enable_if<!std::is_same<Q, bar>::value, bool>::type check()
//    {
//        return false;
//    }

//
//    template< bool cond, typename U >
//    using resolvedType  = typename std::enable_if< cond, U >::type;
//
//public:
//    template< typename U = T >
//    resolvedType< true, U >

    template <typename V = VertexValue, std::enable_if_t<!std::is_same_v<void, V>, int> = 0>
    VertexValue& value() {
        return model.vertices_info[descriptor_].value;
    }
};


}