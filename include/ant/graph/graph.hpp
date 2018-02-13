#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {

template<bool directed_, class NodeType_>
class Graph;

template<bool directed, class NodeType>
Graph<directed, NodeType> Reverse(const Graph<directed, NodeType> &g);


template<bool directed, class NodeType>
class GraphBuilder;

template<bool directed_, class NodeType_>
class Graph {
public:
    // somewhere should be typename
    using NodeType = NodeType_;
    static constexpr bool directed = directed_;

protected:
    std::vector<std::vector<NodeType>> nextNodes_;

public:
    const std::vector<NodeType> &nextNodes(NodeType n) const {
        return nextNodes_[n];
    }

    Count nodeCount() const {
        return nextNodes_.size();
    }

    friend Graph Reverse<directed, NodeType>(const Graph &g);

    friend class GraphBuilder<directed, NodeType>;
    //friend class GraphBuilder<true, NodeType>;

    //friend class GraphBuilder<false, NodeType>;
};


template<bool directed> using Graph32 = Graph<directed, Index>;
template<bool directed> using Graph16 = Graph<directed, int16_t>;

template<bool directed, class NodeType>
class GraphBuilder;

template<class NodeType>
class GraphBuilder<true, NodeType> {

protected:
    Graph<true, NodeType> g_;

public:
    GraphBuilder(Count nodeCount) {
        g_.nextNodes_.resize(nodeCount);
    }

    virtual void Add(NodeType from, NodeType to) {
        g_.nextNodes_[from].push_back(to);
    }

    Graph<true, NodeType> Build() {
        return std::move(g_);
    }
};

template<class NodeType>
class GraphBuilder<false, NodeType> {

    Graph<false, NodeType> g_;

public:
    GraphBuilder(Count nodeCount) {
        g_.nextNodes_.resize(nodeCount);
    }

    void Add(NodeType from, NodeType to) {
        g_.nextNodes_[from].push_back(to);
        g_.nextNodes_[to].push_back(from);
    }

    Graph<false, NodeType> Build() {
        return std::move(g_);
    }
};


template<bool directed, class NodeType>
Graph<directed, NodeType> Reverse(const Graph<directed, NodeType>& g) {
    Graph<directed, NodeType> reversed;
    reversed.nextNodes_.resize(g.nodeCount());
    for (int i = 0; i < g.nodeCount(); ++i) {
        for (auto j : g.nextNodes(i)) {
            reversed.nextNodes_[j].push_back(i);
        }
    }
    return reversed;
}

}