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

struct BasePolicy {
    constexpr static bool directed = false;
};

template<typename P, typename ... Ps>
struct Policy : public P::template WithBase<Policy<Ps...>> {
};

template<typename P>
struct Policy<P> : public P::template WithBase<BasePolicy> {
};

}

}