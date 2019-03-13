#pragma once

#include <variant>

#include "graph_3.hpp"

namespace ant::graph {

    template <typename T_Dist>
    class JunctionRoot_SP {

        struct Elem {
            double dist = std::numeric_limits<T_Dist>::max();
            Index node = -1;
        };

        struct RootItem {
            std::unordered_map<Index, T_Dist> elems;
            std::vector<Index> adj_nodes;
        };

        using NodeItem = std::array<Elem, 2>;

        std::vector<std::variant<NodeItem, RootItem>> nodes;

        // don't need to keep it here, use another struct
        EdgedGraph<Index, Index>& graph;
        std::vector<T_Dist>& edge_dist;

        std::vector<T_Dist> reach;

    public:
        JunctionRoot_SP(EdgedGraph<Index, Index>& graph, std::vector<T_Dist> edge_dist)
            : graph(graph), edge_dist(edge_dist), nodes(graph.nodeCount()), reach(graph.nodeCount()) {

            for (auto i_1 = 0; i_1 < graph.nodeCount(); ++i_1) {
                if (graph.nextNodes(i_1).size() == 2) {
                    // node item
                    NodeItem item;
                    item[0] = {edge_dist[graph.nextEdges(i_1)[0]], graph.nextNodes(i_1)[0]};
                    item[1] = {edge_dist[graph.nextEdges(i_1)[1]], graph.nextNodes(i_1)[1]};
                    nodes[i_1] = item;
                } else {
                    // root item


                }
            }
        }


    };


}