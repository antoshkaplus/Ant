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

    DijkstraComponentsBase<Graph> base;

public:

    // after running the main algorithm
    // will have to price query ability

    explicit DijkstraComponents(Graph& graph) : base(graph) {}

    template <typename RNG>
    void Compute(RNG& rng) {

        auto cluster_count = std::max(1, static_cast<Count>(std::sqrt(CountVertices(base.graph()))));
        auto clusters = CenterClustering(base.graph()).GenerateClusters(cluster_count, rng);
        base.Compute(Clustering{clusters, cluster_count});
    }

    Value Dist(VertexDescriptor from, VertexDescriptor to) const {
        return base.Dist(from, to);
    }
};

}