#pragma once

#include <variant>

#include "graph_3.hpp"
#include "partition.hpp"
#include "ant/core/core.hpp"

namespace ant::graph {

template <typename T_Dist>
class Partition_SP;

template <typename T_Dist>
inline std::ostream& operator<<(std::ostream& out, const Partition_SP<T_Dist>& part);

template <typename T_Dist>
class Partition_SP {

    using Node = Index;

    // don't need to keep it here, use another struct
    const EdgedGraph<Index, Index>& graph;
    const std::vector<T_Dist>& edge_dist;

    struct Sole {};

    struct End {
        Node node;
        bool bound_visited = false;

        End() = default;
        End(Node node) : node(node) {}
    };

    struct Segment {
        std::array<End, 2> ends;
        T_Dist dist = std::numeric_limits<T_Dist>::max();

        End& another(Node end) const {
            return end == ends[0] ? ends[1] : ends[0];
        }

        End& get(Node end) const {
            return end == ends[0] ? ends[0] : ends[1];
        }
    };

    struct Root {
        End origin;
    };

    // node -> unit
    std::vector<Index> nodes_unit;
    // unit -> nodes
    std::vector<std::vector<Node>> unit_nodes;
    std::vector<std::variant<Segment, Sole, Root>> units;

    struct ProgressItem {
        Node to;
        Node from;
        T_Dist dist;

        ProgressItem() = default;
        ProgressItem(Node to, Node from, T_Dist dist)
        : to(to), from(from), dist(dist) {}

        bool operator<(const ProgressItem& item) const {
            return dist > item.dist;
        }
    };

    std::priority_queue<ProgressItem> queue;

    Node target;
    T_Dist target_dist;

    bool stop_search;

    std::vector<T_Dist> reach;

public:
    Partition_SP(const EdgedGraph<Index, Index>& graph, const std::vector<T_Dist>& edge_dist)
        : graph(graph), edge_dist(edge_dist), reach(graph.nodeCount(), std::numeric_limits<T_Dist>::max()) {

        auto res = Partition(graph);
        auto& set_roots = res.node_partition;
        auto& degrees = res.node_degree;

        auto set_count = *std::max_element(set_roots.begin(), set_roots.end()) + 1;

        unit_nodes.clear();
        unit_nodes.resize(set_count);
        for (auto n = 0; n < graph.nodeCount(); ++n) {
            unit_nodes[set_roots[n]].push_back(n);
        }

        units.resize(set_count);
        nodes_unit.resize(graph.nodeCount());

        for (auto i = 0; i < unit_nodes.size(); ++i) {
            std::vector<Node> degree_nodes;
            for (auto n : unit_nodes[i]) {
                nodes_unit[n] = i;
                assert(degrees[n] != 1);
                if (degrees[n] != 0) degree_nodes.push_back(n);
            }

            switch (degree_nodes.size()) {
                case 0: units[i] = Sole{}; break;
                case 1: units[i] = Root{degree_nodes[0]}; break;
                case 2: units[i] = Segment{{degree_nodes[0], degree_nodes[1]},
                                           FindSameUnit(degree_nodes[0], degree_nodes[1])}; break;

                default: throw std::runtime_error("corrupted state");
            }
        }
    }

    std::optional<T_Dist> Query(Node source, Node target) {
        this->target = target;
        target_dist = std::numeric_limits<T_Dist>::max();

        stop_search = false;

        auto& item = units[nodes_unit[source]];
        std::visit([&](auto& item) { InitProgress(item, source); }, item);

        Run();
        for (auto& u : units) {
            std::visit([&](auto& u) { ResetBoundVisited(u); }, u);
        }

        return target_dist;
    }

private:

    void ResetBoundVisited(Segment& s) {
        s.ends[0].bound_visited = false;
        s.ends[1].bound_visited = false;
    }

    void ResetBoundVisited(Root& s) {
        s.origin.bound_visited = false;
    }

    void ResetBoundVisited(Sole&) {}

    // may need to know distance
    T_Dist FindSameUnit(Node source, Node target) {
        std::queue<ProgressItem> q;
        q.emplace(source, -1, 0);

        while (!q.empty()) {
            auto item = q.front();
            q.pop();
            if (reach[item.to] <= item.dist) continue;
            reach[item.to] = item.dist;

            for (auto p : graph.nextPairs(item.to)) {
                if (SameUnit(source, p.node) && p.node != item.from) {
                    q.emplace(p.node, item.to, item.dist + edge_dist[p.edge]);
                }
            }
        }

        auto dist = reach[target];
        for (auto n : unit_nodes[nodes_unit[source]]) {
            reach[n] = std::numeric_limits<T_Dist>::max();
        }
        return dist;
    }

    void InitProgress(Sole& sole, Node source) {
        stop_search = true;
        if (!SameUnit(source, target)) return;

        target_dist = FindSameUnit(source, target);
    }

    void InitProgress(Root& root, Node source) {
        if (SameUnit(source, target)) {
             target_dist = FindSameUnit(source, target);
             stop_search = true;
             return;
        }

        auto dist = FindSameUnit(source, root.origin.node);

        ForBoundNodes(root.origin.node, [&](auto node, auto edge) {
            queue.emplace(node, root.origin.node, dist + edge_dist[edge]);
        });
    }

    void InitProgress(Segment& segment, Node source) {

        if (SameUnit(source, target)) {
            target_dist = FindSameUnit(source, target);
        }

        for (auto i : {0, 1}) {
            auto dist = FindSameUnit(source, segment.ends[i].node);
            ForBoundNodes(segment.ends[i].node, [&] (auto node, auto edge) {
                PushBound(node, segment.ends[i].node, dist + edge_dist[edge]);
            });
        }
    }

    void Run() {
        while (!stop_search && !queue.empty()) {
            auto top = queue.top();
            queue.pop();
            if (top.dist >= target_dist) break;

            auto& item = units[nodes_unit[top.to]];
            std::visit([&](auto& item) {

            // TODO need to do something
                BoundProgress(item, top.dist);
            }, item);
        }
    }

    // Border progress is made straight from the priority queue

    void BoundProgress(const Root& root, T_Dist cur_dist) {
        // comparison prevents from visiting twice
        if (root.origin.bound_visited) return;
        root.origin.bound_visited = true;

        if (SameUnit(root.origin.node, target)) {
            // can leave after that
            target_dist = cur_dist + FindSameUnit(root.origin, target);
            stop_search = true;
            return;
        }

        ForBoundNodes(root.origin.node, [&] (auto node, auto edge) {
            PushBound(node, root.origin.node, cur_dist + edge_dist[edge]);
        });
    }

    void BoundProgress(const Segment& segment, Node end_node, T_Dist cur_dist) {
        auto& end = segment.get(end_node);
        if (end.bound_visited) return;
        end.bound_visited = true;

        if (segment.nodes.count(target) == 1) {
            auto to_target = cur_dist + Find(end_node, target);
            target_dist = std::min(target_dist, to_target);
            if (segment.another(end_node).bound_visited) stop_search = true;

            return;
        }

        auto another = segment.another(end_node);
        auto across_dist = segment.dist;
        ForBoundNodes(another.node, [&] (auto node, auto edge) {
            PushBound(node, another.node, cur_dist + across_dist + edge_dist[edge]);
        });
    }

    template <typename Func>
    void ForBoundNodes(Node node, Func func) {
        for (auto p : graph.nextPairs(node)) {
            if (SameUnit(p.node, node)) continue;
            func(p.node, p.edge);
        }
    }

    void PushBound(Node to, Node from, T_Dist dist) {
        if (dist >= target_dist) return;
        queue.emplace(to, from, dist);
    }

    bool SameUnit(Node n_1, Node n_2) const {
        return nodes_unit[n_1] == nodes_unit[n_2];
    }

    template <typename T>
    friend std::ostream& operator<<(std::ostream& out, const Partition_SP<T>& part);
};

template <typename T_Dist>
inline std::ostream& operator<<(std::ostream& out, const Partition_SP<T_Dist>& part) {
    for (auto i = 0; i < part.units.size(); ++i) {
        Println(out, "Unit: ", i);
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, typename Partition_SP<T_Dist>::Sole>) {
                Println(out, "Sole:");
            }
            else if constexpr (std::is_same_v<T, typename Partition_SP<T_Dist>::Segment>) {
                Println(out, "Segment: ", arg.ends[0].node, " ", arg.ends[0].bound_visited, " ",
                                          arg.ends[1].node, " ", arg.ends[1].bound_visited, " dist: ", arg.dist);
            }
            else if constexpr (std::is_same_v<T, typename Partition_SP<T_Dist>::Root>) {
                Println(out, "Root: ", arg.origin.node, " ", arg.origin.bound_visited);
            }
//            else
//                static_assert(false, "non-exhaustive visitor!");
        }, part.units[i]);
        Println(out, part.unit_nodes[i]);

    }
    return out;
}


}