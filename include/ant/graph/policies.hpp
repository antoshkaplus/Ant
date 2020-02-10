#pragma once

#include "ant/graph/graph_traits.hpp"

namespace ant::graph {

namespace policy {

struct Directed {
    template<typename Base>
    struct WithBase : Base {
        constexpr static bool directed = true;
    };
};

template<typename VertexValue_>
struct VertexValue {
    template<typename Base>
    struct WithBase : Base {
        using VertexValue = VertexValue_;
    };
};

template <typename IndexType = Index>
struct EdgeIndexDescriptor {
    template<typename Base>
    struct WithBase : Base {
        using EdgeDescriptor = IndexType;
        constexpr static bool edge_descriptor_index = true;
    };
};

struct BasePolicy {
    constexpr static bool directed = false;

    using VertexDescriptor = Index;
    using VertexValue = void;
    constexpr static bool vertex_descriptor_index = true;
};

template<typename P, typename ... Ps>
struct Policy : public P::template WithBase<Policy<Ps...>> {
};

template<typename P>
struct Policy<P> : public P::template WithBase<BasePolicy> {
};

}

}