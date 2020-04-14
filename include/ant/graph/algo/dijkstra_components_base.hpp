#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/graph.hpp"
#include "cluster.hpp"
#include "bfs.hpp"
#include "dijkstra.hpp"


namespace ant::graph {

template <typename Graph>
class DijkstraComponents;

enum class DijkstraComponentsBase_ComponentDistances {
    Dijkstra,
    Linear
};

template<typename Graph, DijkstraComponentsBase_ComponentDistances kComponentDistances>
class DijkstraComponentsBase {
    using VertexDescriptor = typename Graph::VertexDescriptor;
    using Value = typename Graph::EdgeValue;
    using ComponentPolicy = ant::graph::policy::Policy<policy::EdgeValue<Value>>;
    using ComponentGraph = ant::graph::Graph<ComponentPolicy>;

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

    grid::Grid<Value> min_component_distance_;

    // buffer when search Dist
    std::vector<bool> visited;

    friend class DijkstraComponents<Graph>;
public:

    // after running the main algorithm
    // will have to price query ability

    explicit DijkstraComponentsBase(Graph& graph) : graph_(graph), visited(CountVertices(graph_)) {}

    auto min_component_distance(Index c_1, Index c_2) const {
        return min_component_distance_(c_1, c_2);
    }

    auto& graph() {
        return graph_;
    }

    // cluster can't have a loop if using Linear
    // also should be connected
    void Compute(const Clustering& clustering) {

        // should compute all the components
        vertices_component_info.clear();
        vertices_component_info.resize(CountVertices(graph_));

        components.clear();
        components.resize(clustering.cluster_count);

        auto& clusters = clustering.vertex_cluster;
        std::vector<Index> cluster_current_size(components.size());
        for (auto i = 0; i < clusters.size(); ++i) {
            vertices_component_info[i].component = clusters[i];
            vertices_component_info[i].component_index = cluster_current_size[clusters[i]]++;

            components[clusters[i]].vertices_index.push_back(i);
        }
        for (auto i = 0; i < components.size(); ++i) {
            components[i].distances.resize(cluster_current_size[i], cluster_current_size[i]);
        }

        std::vector<ComponentGraph> component_graphs(cluster_current_size.size());
        for (auto i = 0; i < cluster_current_size.size(); ++i) {
            auto& g = component_graphs[i];
            typename ant::graph::Graph<ComponentPolicy>::Mutator mutator(g);
            mutator.SetVertexCount(cluster_current_size[i]);
        }

        for (auto&& e : graph_.edges()) {
            auto [from_v, to_v] = e.vertices();

            auto from = from_v.descriptor();
            auto to = to_v.descriptor();

            auto from_cluster = clusters[from];
            auto to_cluster = clusters[to];

            if (from_cluster != to_cluster) {
                components[from_cluster].borders.insert(vertices_component_info[from].component_index);
                components[to_cluster].borders.insert(vertices_component_info[to].component_index);
            } else {
                typename ant::graph::Graph<ComponentPolicy>::Mutator mutator(component_graphs[from_cluster]);
                mutator.AddEdge(vertices_component_info[from].component_index,
                                vertices_component_info[to].component_index,
                                e.value());
            }
        }

        for (auto cluster_index = 0; cluster_index < cluster_current_size.size(); ++cluster_index) {
            if constexpr (kComponentDistances == DijkstraComponentsBase_ComponentDistances::Dijkstra) {
                ComputeComponentDistances_Dijkstra(cluster_index, component_graphs[cluster_index],
                                                   components[cluster_index].distances);
            }
            if constexpr (kComponentDistances == DijkstraComponentsBase_ComponentDistances::Linear) {
                ComputeComponentDistances_Linear(cluster_index, component_graphs[cluster_index],
                                                   components[cluster_index].distances);
            }
        }

        min_component_distance_.resize(cluster_current_size.size(), cluster_current_size.size());

        for (auto i = 0; i < components.size(); ++i) {
            auto distances = ComputeInterComponentDistances(i);
            for (auto k = 0; k < distances.size(); ++k) {
                min_component_distance_(i, k) = distances[k];
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
    // initial distanation consists of two components: lower bound estimate left to go + how much went through
    // lower bound is found earlier as distance between components
    // once solution found we will have to drain the queue while estimates there still satisfy new solution
    // constraint
    template <bool verbose = false>
    Value Dist(VertexDescriptor from, VertexDescriptor to) const {

        struct Item {
            VertexDescriptor to;
            Value value;
            Value estimate;

            Item() = default;
            Item(VertexDescriptor to, Value value, Value estimate)
                : to(to), value(value), estimate(estimate) {}

            // priority queue is max queue by default
            bool operator<(const Item& item) const {
                return estimate > item.estimate;
            }
        };

        auto PrintItem = [](const Item& item) {
            Println(std::cout, item.to, " v:", item.value, " e:", item.estimate);
        };

        auto from_comp = vertices_component_info[from].component;
        auto to_comp = vertices_component_info[to].component;

        auto from_comp_index = vertices_component_info[from].component_index;
        auto to_comp_index = vertices_component_info[to].component_index;

        Value current_solution = std::numeric_limits<Value>::max();

        if (from_comp == to_comp) {
            // even if the same component could find something while going through another component
            current_solution = components[from_comp].distances(from_comp_index, to_comp_index);
        }

        std::vector<Value> visited_distance(vertices_component_info.size(), std::numeric_limits<Value>::max());

        std::priority_queue<Item> q;
        for (auto b : components[from_comp].borders) {
            q.emplace(components[from_comp].vertices_index[b],
                      components[from_comp].distances(b, from_comp_index),
                      components[from_comp].distances(b, from_comp_index) + min_component_distance_(from_comp, to_comp));

            if constexpr (verbose) PrintItem(q.top());
        }

        while (!q.empty()) {
            Item t = q.top();
            q.pop();
            if (visited_distance[t.to] < t.value) continue;
            visited_distance[t.to] = t.value;

            if (current_solution <= t.estimate) {
                if constexpr (verbose) Println(std::cout,"early break");
                break;
            }

            auto comp = vertices_component_info[t.to].component;
            if (comp == to_comp) {
                // reached target component
                auto new_solution = t.value + components[comp].distances(
                        vertices_component_info[t.to].component_index,
                        to_comp_index);
                if (new_solution < current_solution) {
                    current_solution = new_solution;
                    if constexpr (verbose) Println(std::cout, "sol: ", new_solution);
                }
                // maybe can do better by moving into another component still
            }

            // expand to borders
            auto& comp_obj = components[comp];
            for (auto b : comp_obj.borders) {
                auto b_vertex = comp_obj.vertices_index[b];
                auto dist = t.value + comp_obj.distances(vertices_component_info[t.to].component_index, b);
                if (dist < visited_distance[b_vertex]) {
                    visited_distance[b_vertex] = dist;
                    q.emplace(b_vertex, dist, dist + min_component_distance_(comp, to_comp));

                    if constexpr (verbose) PrintItem(q.top());
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
                    auto estimate = min_component_distance_(
                            vertices_component_info[p.to().descriptor()].component, to_comp);
                    q.emplace(p.to().descriptor(), dist, dist + estimate);

                    if constexpr (verbose) PrintItem(q.top());
                }
            }
        }

        return current_solution;
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

    std::vector<Value> ComputeInterComponentDistances(Index component_index) {
        struct Item {
            VertexDescriptor to;
            Value value;

            Item() = default;
            Item(VertexDescriptor to, Value value) : to(to), value(value) {}

            bool operator<(const Item& item) const {
                return value > item.value;
            }
        };

        std::vector<Value> result(components.size(), std::numeric_limits<Value>::max());
        std::vector<Value> visited_distance(vertices_component_info.size(), std::numeric_limits<Value>::max());

        std::priority_queue<Item> q;
        for (auto b : components[component_index].borders) {
            q.emplace(components[component_index].vertices_index[b], 0);
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

    void ComputeComponentDistances_Dijkstra(Index component_index, ComponentGraph& g, grid::Grid<Value>& distances) {
        Dijkstra dijkstra(g);
        for (auto i = 0; i < CountVertices(g); ++i) {
            auto ds = std::get<0>(dijkstra.Compute(i));
            for (auto j = 0; j < CountVertices(g); ++j) {
                // doing double work with those distances
                distances(i, j) = ds[j];
            }
        }
    }

    void ComputeComponentDistances_Linear(Index component_index, ComponentGraph& g, grid::Grid<Value>& distances) {
        distances.fill(std::numeric_limits<Value>::max());

        auto size = CountVertices(g);
        for (auto i = 0; i < size; ++i) distances(i, i) = 0;

        struct Item {
            Index from;
            Index to;
        };

        std::queue<Item> q;
        std::vector<Index> visited_vertices;
        visited_vertices.push_back(0);
        q.push({0, 0});

        while (!q.empty()) {
            auto item = q.front();
            q.pop();
            for (auto a : g.vertex(item.to).advance()) {
                if (item.from == a.to().descriptor()) {
                    continue;
                }

                // new edge
                for (auto v : visited_vertices) {
                    // to coop with double edges
                    distances(v, a.to().descriptor()) = distances(a.to().descriptor(), v) =
                        std::min(a.edge().value() + distances(v, item.to), distances(v, a.to().descriptor()));
                }
                visited_vertices.push_back(a.to().descriptor());
                q.push({item.to, a.to().descriptor()});
            }
        }
    }
};

}