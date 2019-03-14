#pragma once

#include <variant>

#include "graph_3.hpp"

namespace ant::graph {

    template <typename T_Dist>
    class JunctionRoot_SP {

        struct Line {
            Index source_root;
            Index target_root;

            std::unordered_map<Index, T_Dist> dist;
        };

        // all the lines
        std::vector<Line> lines;

        using Node = Index;
        // index of the line
        using LineIndex = Index;
        // another root -> Line index
        using Root = std::unordered_map<Node, LineIndex>;

        std::vector<std::variant<LineIndex, Root>> nodes;

        // don't need to keep it here, use another struct
        EdgedGraph<Index, Index>& graph;
        std::vector<T_Dist>& edge_dist;

        std::vector<T_Dist> reach;

    public:
        JunctionRoot_SP(EdgedGraph<Index, Index>& graph, std::vector<T_Dist> edge_dist)
            : graph(graph), edge_dist(edge_dist), nodes(graph.nodeCount()), reach(graph.nodeCount()), nodes(graph.nodeCount()) {

            auto root_count = 0;
            for (auto i = 0; i < graph.nodeCount(); ++i) {
                if (graph.nextNodes(i).size() == 2) {
                    // node Item
                    nodes[i] = LineIndex{-1};
                } else {
                    // root Item
                    nodes[i] = Root{};
                    ++root_count;
                }
            }

            // make artificial roots if don't have any
            if (root_count == 0) {
                nodes[0] = Root{};
                graph.nextNodes(0)[0] = Root{};
            }

            // now need to initialize everything
            for (auto i = 0; i < graph.nodeCount(); ++i) {
                if (std::holds_alternative<Root>(nodes[i])) {

                    for (auto p : graph.nextPairs(i)) {
                        BuildLine(p.node, i, edge_dist[p.edge]);
                    }

                }
            }
        }

        // builds line in a given direction
        void BuildLine(Node n, Node from_root_node, T_Dist dist) {
            auto& from_root = std::get<Root>(nodes[from_root_node]);

            auto new_line_index = lines.size();
            Line line;
            line.source_root = from_root_node;

            auto from = from_root_node;
            for (;;) {

                line.dist[n] = dist;
                if (std::holds_alternative<Root>(nodes[n])) {

                    // already computed
                    if (from_root.count(n) == 1) return;

                    auto& to_root = std::get(nodes[n]);
                    to_root[from_root_node] = new_line_index;
                    from_root[n] = new_line_index;

                    line.target_root = n;

                    lines.push_back(line);
                    return;
                }

                auto& line_index = std::get<LineIndex>(nodes[n]);
                // already computed
                if (line_index != -1) return;

                line_index = new_line_index;

                tie(from, tie(n, e)) = tiple(n, another(n, from));
                dist += edge_dist[e];
            }
        }

    private:
        tuple<Node, Index> another(Node n, Node from) {
            auto& ns = graph.nextNodes(n);
            auto& es = graph.nextEdges(n);
            return ns[0]. == from ? {ns[1], es[1]} : {ns[0], es[0]};
        }

    };



}