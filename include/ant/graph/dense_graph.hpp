#pragma once

// NEVER worth using if want to traverse nextNodes

#include <ant/core/core.hpp>

namespace ant::graph {

template<bool directed>
class DenseGraph;

DenseGraph<true> Reverse(const DenseGraph<true>& g);


template<bool directed>
class DenseGraphBuilder;

template<bool directed_>
class DenseGraph {
public:
    // somewhere should be typename
    using NodeType = Index;
    static constexpr bool directed = directed_;

protected:
    std::vector<char> grid;
    std::vector<NodeType> degrees;

private:
    DenseGraph(NodeType nodeCount)
            : grid(nodeCount * nodeCount, false), degrees(nodeCount) {}

    Index index(NodeType from, NodeType to) {
        return from * nodeCount() + to;
    }

public:
    template<class Func>
    void forEachNextNode(NodeType n, Func&& func) const {
        auto start = n * nodeCount();
        for (auto i = 0; i < nodeCount(); ++i) {
            if (grid[start + i]) func(i);
        }
    }

    bool hasEdge(NodeType from, NodeType to) {
        return grid[index(from, to)];
    }

    NodeType nodeCount() const {
        return static_cast<NodeType>(degrees.size());
    }

    NodeType degree(NodeType n) const {
        return static_cast<NodeType>(degrees[n]);
    }

    template<class Func>
    friend void ForEachEdge(const DenseGraph<true>& graph, Func&& func);
    template<class Func>
    friend void ForEachEdge(const DenseGraph<false>& graph, Func&& func);

    friend DenseGraph<true> Reverse(const DenseGraph<true>& g);

    friend class DenseGraphBuilder<directed>;
};

template<bool directed>
class DenseGraphBuilder;

template<>
class DenseGraphBuilder<true> {
public:
    using GraphType = DenseGraph<true>;
    using NodeType = GraphType::NodeType;

private:
    DenseGraph<true> g_;

public:
    DenseGraphBuilder(Count nodeCount) : g_(nodeCount) {}

    virtual void Add(NodeType from, NodeType to) {
        g_.grid[g_.index(from, to)] = true;
    }

    DenseGraph<true> Build() {
        return std::move(g_);
    }
};

template<>
class DenseGraphBuilder<false> {
public:
    using GraphType = DenseGraph<false>;
    using NodeType = GraphType::NodeType;

private:
    DenseGraph<false> g_;

public:
    DenseGraphBuilder(Count nodeCount) : g_(nodeCount) {}

    void Add(NodeType n_1, NodeType n_2) {
        g_.grid[g_.index(n_1, n_2)] = true;
        g_.grid[g_.index(n_2, n_1)] = true;
    }

    DenseGraph<false> Build() {
        return std::move(g_);
    }
};


template<class Func>
void ForEachEdge(const DenseGraph<false>& graph, Func&& func) {
    for (auto i = 0; i < graph.nodeCount(); ++i) {
        auto start = i * graph.nodeCount();
        for (auto j = i; j < graph.nodeCount(); ++j) {
            if (graph.grid[start + j]) func(i, j);
        }
    }
}

template<class Func>
void ForEachEdge(const DenseGraph<true>& graph, Func&& func) {
    for (auto i = 0; i < graph.nodeCount(); ++i) {
        auto start = i * graph.nodeCount();
        for (auto j = 0; j < graph.nodeCount(); ++j) {
            if (graph.grid[start + j]) func(i, j);
        }
    }
}


DenseGraph<true> Reverse(const DenseGraph<true>& g) {
    DenseGraphBuilder<true> builder(g.nodeCount());
    ForEachEdge(g, [&](auto from, auto to) {
        builder.Add(to, from);
    });
    return builder.Build();
}

}