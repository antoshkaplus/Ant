#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/model/graph_3.hpp"
#include "ant/graph/bfs_3.hpp"
#include "ant/graph/cluster.hpp"


namespace ant::graph {

template <typename Graph>
class DijkstraComponents {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Value = typename Graph::EdgeValue;

    struct ComponentInfo {
        // which component
        Index component;
        // mapping to index inside component
        Index component_index;
    };

    struct Component {
        std::unordered_set<Index> borders;
        std::vector<Index> vertices_index;
        grid::Grid<Value> distances;
    };

    Graph& graph_;

    std::vector<ComponentInfo> vertices_component_info;
    std::vector<Component> components;

public:

    // after running the main algorithm
    // will have to price query ability

    DijkstraComponents(Graph& graph) : graph_(graph) {}

    template <typename RNG>
    void Compute(RNG& rng) {

        // should compute all the components
        vertices_component_info.clear();
        vertices_component_info.resize(CountVertices(graph_));

        components.clear();
        components.resize(std::max(1, static_cast<Count>(std::sqrt(vertices_component_info.size()))));

        auto clusters = CenterClustering(graph_).GenerateClusters(components.size(), rng);
        std::vector<Index> cluster_current_size(components.size());
        for (auto i = 0; i < clusters.size(); ++i) {
            vertices_component_info[i].component = clusters[i];
            vertices_component_info[i].component_index = cluster_current_size[clusters[i]]++;
        }

        std::vector<ant::graph::model::Graph<>>

//        std::vector<>
//        for (auto i = 0; i < )


        // now find border vertices


        // now find distances

    }

    // should return optional to have existance check ???
    Value Dist(VertexDescriptor from, VertexDescriptor to) const {
        return dist_(from, to);
    }

};

}