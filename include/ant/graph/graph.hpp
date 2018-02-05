#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {

template<bool directed, class NodeType>
class Graph {
public:
    // somewhere should be typename
    using NodeType = NodeType;

protected:
    std::vector<std::vector<NodeType>> nextNodes_;

public:
    const std::vector<NodeType>& nextNodes(NodeType n) const {
        return nextNodes_[n];
    }

    Count nodeCount() const {
        return nextNodes_.size();
    }

    Graph<directed, NodeType> reversed() const {
        Graph<directed, NodeType> rG;
        rG.nextNodes_.resize(nodeCount());
        for (int i = 0; i < nodeCount(); ++i) {
            for (auto j : nextNodes(i)) {
                rG.nextNodes_[j].push_back(i);
            }
        }
        return rG;
    }
};

template<bool directed> using Graph32 = Graph<directed, Index>;
template<bool directed> using Graph16 = Graph<directed, Index>;

template<bool directed, class NodeType>
class GraphBuilder;

template<class NodeType>
class GraphBuilder<true, NodeType> {

};

template<class NodeType>
class GraphBuilder<false, NodeType> {

};


template<bool directed, class NodeType, class EdgeType>
class 


}