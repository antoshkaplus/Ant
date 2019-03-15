#pragma once

#include <variant>

#include "graph_3.hpp"

namespace ant::graph {

    template <typename T_Dist>
    class JunctionRoot_SP {

        using Node = Index;
        // index of the line
        using LineIndex = Index;
        using Root = std::vector<LineIndex>;

        // source and target can be equal
        struct Line {
            Node source_root;
            Node target_root;

            std::unordered_map<Node, T_Dist> dist;
        };

        // all the lines
        std::vector<Line> lines;

        std::vector<std::variant<LineIndex, Root>> nodes;

        // don't need to keep it here, use another struct
        EdgedGraph<Index, Index>& graph;
        std::vector<T_Dist>& edge_dist;

        std::vector<T_Dist> reach;

    public:
        JunctionRoot_SP(EdgedGraph<Index, Index>& graph, std::vector<T_Dist> edge_dist)
            : graph(graph), edge_dist(edge_dist), nodes(graph.nodeCount()), reach(graph.nodeCount()) {


        }


    };


}