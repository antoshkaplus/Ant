#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/graph.hpp"
#include "cluster.hpp"
#include "bfs.hpp"
#include "dijkstra.hpp"
#include "dijkstra_components_base.hpp"


namespace ant::graph {

template <typename Graph>
class DijkstraComponents {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Value = typename Graph::EdgeValue;

    DijkstraComponentsBase<Graph, DijkstraComponentsBase_ComponentDistances::Linear> base;

public:

    // after running the main algorithm
    // will have to price query ability

    explicit DijkstraComponents(Graph& graph) : base(graph) {}

    template <typename RNG>
    void Compute(RNG& rng) {

        // should have zero cycles
        auto cluster_count = std::max(1, static_cast<Count>(std::sqrt(CountVertices(base.graph()))));
        auto clustering = TrivialClustering_NoLoops(base.graph(), cluster_count, rng);
        base.Compute(clustering);
    }

    template <typename RNG>
    void Precompute(RNG& rng) {
        Compute(rng);
    }


    Value Dist(VertexDescriptor from, VertexDescriptor to) const {
        return base.Dist(from, to);
    }

    Value Compute(VertexDescriptor from, VertexDescriptor to) const {
        return base.Dist(from, to);
    }
};

}