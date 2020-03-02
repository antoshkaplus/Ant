#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/cluster.hpp"
#include "ant/graph/graph.hpp"
#include "bfs.hpp"
#include "dijkstra.hpp"


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

    grid::Grid<Value> min_component_distance;

    // buffer when search Dist
    std::vector<bool> visited;

public:

    // after running the main algorithm
    // will have to price query ability

    explicit DijkstraComponents(Graph& graph) : graph_(graph), visited(CountVertices(graph_)) {}

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

            components[clusters[i]].vertices_index.push_back(i);
        }
        for (auto i = 0; i < components.size(); ++i) {
            components[i].distances.resize(cluster_current_size[i], cluster_current_size[i]);
        }

        using Policy = ant::graph::policy::Policy<policy::EdgeValue<Value>>;
        std::vector<ant::graph::Graph<Policy>> component_graphs(cluster_current_size.size());
        for (auto i = 0; i < cluster_current_size.size(); ++i) {
            auto& g = component_graphs[i];
            typename ant::graph::Graph<Policy>::Mutator mutator(g);
            mutator.SetVertexCount(cluster_current_size[i]);
        }
        BFS_Prev(graph_, 0, [&](auto& from_vertex, auto& advance) {
            auto from = from_vertex.descriptor();
            auto to = advance.to().descriptor();

            auto from_cluster = clusters[from];
            auto to_cluster = clusters[to];

            if (from_cluster != to_cluster) {
                components[from_cluster].borders.insert(vertices_component_info[from].component_index);
                components[to_cluster].borders.insert(vertices_component_info[to].component_index);
            } else {
                component_graphs[from_cluster].AddEdge(vertices_component_info[from].component_index,
                                                       vertices_component_info[to].component_index,
                                                       advance.edge().value());
            }
        });

        for (auto cluster_index = 0; cluster_index < cluster_current_size.size(); ++cluster_index) {
            auto& g = component_graphs[cluster_index];
            Dijkstra dijkstra(g);
            for (auto i = 0; i < cluster_current_size[cluster_index]; ++i) {
                auto distances = std::get<0>(dijkstra.Compute(i));
                for (auto j = 0; j < cluster_current_size[cluster_index]; ++j) {
                    // doing double work with those distances
                    components[cluster_index].distances(i, j) = distances[j];
                }
            }
        }

        min_component_distance.resize(cluster_current_size.size(), cluster_current_size.size());

        for (auto i = 0; i < components.size(); ++i) {
            auto distances = ComputeComponentDistances(i);
            for (auto k = 0; k < distances.size(); ++k) {
                min_component_distance(i, k) = distances[k];
            }
        }
    }

    // should have lower bound estimate (LBE) = max() at first

    // 0) check "to" in the same component, return result
    // 1) push "from" borders inside the q with dist
    //      when pushing try update LBE with distance(from_comp, to_comp) + distance(from to border)
    // 2) pop
    // 3) try visit all borders, make sure that it's still good in terms of LBE
    // 4) try visit another components by edges, then make sure LBE holds
    // 5) with progress LBE will be growing???
    //      so the priority queue has to be sorted by LBE that each item has.

    //
    Value Dist(VertexDescriptor from, VertexDescriptor to) const {

        struct Item {
            VertexDescriptor to;
            Value value;
            Value estimate;
        };

        auto from_comp = vertices_component_info[from].component;
        auto to_comp = vertices_component_info[to].component;

        auto from_comp_index = vertices_component_info[from].component_index;
        auto to_comp_index = vertices_component_info[to].component_index;

        if (from_comp == to_comp) {
            return components[from_comp].distances(from_comp_index, to_comp_index);
        }

        std::vector<Value> visited_distance(vertices_component_info.size(), std::numeric_limits<Value>::max());

        // TODO override comparison
        std::priority_queue<Item> q;
        for (auto b : components[from_comp].borders) {
            q.emplace(components[from_comp].vertex_index[b],
                    components[from_comp].distances(b, from_comp_index));
        }

        Value min_estimate = std::numeric_limits<Value>::max();
        while (!q.empty()) {
            Item t = q.top();
            q.pop();
            if (visited_distance[t.to] < t.value) continue;
            visited_distance[t.to] = t.value;

            min_estimate = t.estimate;
            auto comp = vertices_component_info[t.to].component;
            if (comp == to_comp) {

            }

            // expand to borders
            auto& comp_obj = components[comp];
            for (auto b : comp_obj.borders) {
                auto b_vertex = comp_obj.vertices_index[b];
                auto dist = t.value + comp_obj.distances(vertices_component_info[t.to].component_index, b);
                if (dist < visited_distance[b_vertex]) {
                    visited_distance[b_vertex] = dist;
                    q.emplace(b_vertex, dist);
                }
            }

            // expand over border
            for (auto p : graph_.vertex(t.to).advance()) {
                if (vertices_component_info[t.to].component == vertices_component_info[p.to().descriptor()].component) {
                    continue;
                }

                auto dist = t.value + p.edge().value();
                if (dist < visited_distance[p.to().descriptor()]) {
                    visited_distance[p.to().descriptor()] = dist;
                    q.emplace(p.to().descriptor(), dist);
                }
            }
        }
    }


private:

    // on visit we see
    // if visited == true => do nothing
    // if component distance is not set => set new component distance

    // 1) try go to borders of current component
    // 2) try to go to another component with edges

    // many points get added without much thought.
    // to alleviate that we should keep a distance on each point instead of visited bool array
    // that way we won't comeback if new distance is too big
    // before putting element into the queue we update distance on items
    // edge values have to be more than 0 to be able to come on equality

    std::vector<Value> ComputeComponentDistances(Index component_index) {
        struct Item {
            VertexDescriptor to;
            Value value;
        };

        std::vector<Value> result(components.size(), std::numeric_limits<Value>::max());
        std::vector<Value> visited_distance(vertices_component_info.size(), std::numeric_limits<Value>::max());

        std::priority_queue<Item> q;
        for (auto b : components[component_index].borders) {
            q.emplace(components[component_index].vertex_index[b], 0);
        }
        while (!q.empty()) {
            Item t = q.top();
            q.pop();
            if (visited_distance[t.to] < t.value) continue;
            visited_distance[t.to] = t.value;

            // check for component distance
            if (result[vertices_component_info[t.to].component] > t.value) {
                result[vertices_component_info[t.to].component] = t.value;
            }

            auto comp = vertices_component_info[t.to].component;
            auto to_comp_index = vertices_component_info[t.to].component_index;

            // expand to borders
            auto& comp_obj = components[comp];
            for (auto b : comp_obj.borders) {
                auto b_vertex = comp_obj.vertices_index[b];
                auto dist = t.value + comp_obj.distances(to_comp_index, b);
                if (dist < visited_distance[b_vertex]) {
                    visited_distance[b_vertex] = dist;
                    q.emplace(b_vertex, dist);
                }
            }

            // expand over border
            for (auto p : graph_.vertex(t.to).advance()) {
                if (vertices_component_info[t.to].component == vertices_component_info[p.to().descriptor()].component) {
                    continue;
                }

                auto dist = t.value + p.edge().value();
                if (dist < visited_distance[p.to().descriptor()]) {
                    visited_distance[p.to().descriptor()] = dist;
                    q.emplace(p.to().descriptor(), dist);
                }
            }
        }
        return result;
    }
};

}