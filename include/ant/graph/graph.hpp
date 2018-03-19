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

    NodeType nodeCount() const {
        return static_cast<NodeType>(nextNodes_.size());
    }

    NodeType degree(NodeType n) const {
        return static_cast<NodeType>(nextNodes_[n].size());
    }

    template<class Func> friend void ForEachEdge(Graph& graph, Func&& func);
    friend Graph Reverse<directed, NodeType>(const Graph &g);
    friend class GraphBuilder<directed, NodeType>;
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

template<class Graph, class Func, typename std::enable_if<Graph::directed==false>::type* = nullptr>
void ForEachEdge(const Graph& graph, Func&& func) {
    for (auto i = 0; i < graph.nodeCount(); ++i) {
        for (auto j : graph.nextNodes(i)) {
            if (i > j) continue;

            func(i, j);
        }
    }
}

template<class Graph, class Func, typename std::enable_if<Graph::directed==true>::type* = nullptr>
void ForEachEdge(const Graph& graph, Func&& func) {
    for (auto i = 0; i < graph.nodeCount(); ++i) {
        for (auto j : graph.nextNodes(i)) {
            func(i, j);
        }
    }
}

template<bool directed, class NodeType, typename std::enable_if<directed==false>::type* = nullptr>
Graph<directed, NodeType> BuildRandom(NodeType nodeCount, double completeness) {
    // very stupid algorithm actually
    std::vector<std::pair<NodeType, NodeType>> edges;
    for (auto i = 0; i < nodeCount; ++i) {
        for (auto j = i+1; j < nodeCount; ++j) {
            edges.emplace_back(i, j);
        }
    }
    std::default_random_engine rng(time(0));
    std::shuffle(edges.begin(), edges.end(), rng);
    Count edgeCountNeeded = completeness*(nodeCount*nodeCount - nodeCount)/2;
    edges.erase(edges.begin() + edgeCountNeeded, edges.end());

    GraphBuilder<directed, NodeType> builder(nodeCount);
    for (auto [i, j] : edges) {
        builder.Add(i, j);
    }
    return builder.Build();
};


}

//static Graph random(Count nodeCount, double completeness) {
//    AdjacencyList adjList(nodeCount);
//    // very stupid algorithm actually
//    vector<Edge> edges;
//    for (auto i = 0; i < nodeCount; ++i) {
//        for (auto j = i+1; j < nodeCount; ++j) {
//            edges.emplace_back(i, j);
//        }
//    }
//    std::random_shuffle(edges.begin(), edges.end());
//    Count edgeCountNeeded = completeness*(nodeCount*nodeCount - nodeCount)/2;
//    edges.erase(edges.begin() + edgeCountNeeded, edges.end());
//    return Graph(edges, nodeCount);
//}
//
//AdjacencyList Graph::edgesToAdjacencyList(const vector<Edge>& edges, size_t nodeCount) {
//    AdjacencyList adjList(nodeCount);
//    for (const Edge& p : edges) {
//        adjList[p.first].push_back(p.second);
//        adjList[p.second].push_back(p.first);
//    }
//    return adjList;
//}
