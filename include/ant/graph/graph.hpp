#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {


template <typename Graph, typename = void>
struct has_vertex_id_index : std::false_type {};
template <typename Graph>
struct has_vertex_id_index<Graph, std::void_t<decltype(&Graph::vertex_id_index)>> : std::true_type {};

template <typename Graph>
struct is_vertex_id_index {
    constexpr static bool value = has_vertex_id_index<Graph>::value && Graph::vertex_id_index;
};
template <typename Graph>
inline constexpr bool is_vertex_id_index_v = is_vertex_id_index<Graph>::value;


template <typename Graph, typename = void>
struct has_directed : std::false_type {};
template <typename Graph>
struct has_directed<Graph, std::void_t<decltype(&Graph::directed)>> : std::true_type {};

template <typename Graph>
struct is_directed {
    constexpr static bool value = has_directed<Graph>::value && Graph::directed;
};
template <typename Graph>
inline constexpr bool is_directed_v = is_directed<Graph>::value;



/*

 Minimum Graph interface

    Graph {
        using NodeType;

        bool directed() - can be constexpr method, should we make it static ???

        node_count()

        edges() - should return struct with begin() and end() methods

    }

    GraphBuilder {
        using GraphType;

        void Add()
        Graph Build()
    }

    GraphMutator {
        // should be able to remove / add edges
        // should be able to remove / add vertices
    }

    How to pass in the mutator guard ?? MutatorGuard mutator???

    Guard Mutator Type has to be hidden inside type


    Edge {
        NodeIndex nodeIndex;
        EdgeIndex edgeIndex;
        [[no_unique_address]] EdgeValue value;
    }

    Node {
        std::vector<Edge> edges;
        [[no_unique_address]] VertixValue value;
    }

    EdgeInfo {
        VertixIndex source;
        Index sourceEdgeIndex;

        VertixIndex target;
    }

    Graph {
        vector<Node> nodes;
        vector<EdgeInfo> edgeInfos;
    }

    when things have the same interface but differ in representation, then it makes sense to
    use special ParamsType

    // FIRST OPEN THE INTERFACE AGAIN TO ALLOW AS MANY CHANGES AS POSSIBLE
    // FROM THERE CAN TO FACADE

    so you have a bunch of Graph templates with GraphCategory that goes in,
    then implementations are picked based on Category

    User graphs just have to have Categories inside them too, user can use Adapter if needed
    some properties could be derived, so to User we give one set of properties and Implementations will use another

    Algorithms shouldn't probably care much, only interface can change... really ???

    Base can also help to pick up common functionality

    Facades help to granulate interfaces

    IF you want to mutate the graph a lot, have to think over more
*/

template<bool directed_, class NodeType_>
class Graph;

template<class NodeType>
Graph<true, NodeType> Reverse(const Graph<true, NodeType> &g);


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

    template<class Func>
    void forEachNextNode(NodeType n, Func&& func) {
        for (auto i : nextNodes_[n]) {
            func(i);
        }
    }

    template<class Func>
    void forEachNextNode(NodeType n, Func&& func) const {
        for (auto i : nextNodes_[n]) {
            func(i);
        }
    }

    friend Graph<true, NodeType> Reverse<NodeType>(const Graph<true, NodeType>& g);
    friend class GraphBuilder<directed, NodeType>;
};


template<bool directed> using Graph32 = Graph<directed, Index>;
template<bool directed> using Graph16 = Graph<directed, int16_t>;

template<bool directed, class NodeType>
class GraphBuilder;

template<class NodeType_>
class GraphBuilder<true, NodeType_> {
public:
    using GraphType = Graph<true, NodeType_>;
    using NodeType = NodeType_;

protected:
    GraphType g_;

public:
    GraphBuilder(Count nodeCount) {
        g_.nextNodes_.resize(nodeCount);
    }

    virtual void Add(NodeType from, NodeType to) {
        g_.nextNodes_[from].push_back(to);
    }

    GraphType Build() {
        return std::move(g_);
    }
};

template<class NodeType_>
class GraphBuilder<false, NodeType_> {
public:
    using GraphType = Graph<false, NodeType_>;
    using NodeType = NodeType_;

private:
    GraphType g_;

public:
    GraphBuilder(Count nodeCount) {
        g_.nextNodes_.resize(nodeCount);
    }

    void Add(NodeType from, NodeType to) {
        g_.nextNodes_[from].push_back(to);
        g_.nextNodes_[to].push_back(from);
    }

    GraphType Build() {
        return std::move(g_);
    }
};


template<class Graph, class Func, typename std::enable_if<Graph::directed==false>::type* = nullptr>
void ForEachEdge(const Graph& graph, Func&& func) {
    for (typename Graph::NodeType i = 0; i < graph.nodeCount(); ++i) {
        for (auto j : graph.nextNodes(i)) {
            if (i > j) continue;

            func(i, j);
        }
    }
}

template<class Graph, class Func, typename std::enable_if<Graph::directed==true>::type* = nullptr>
void ForEachEdge(const Graph& graph, Func&& func) {
    for (typename Graph::NodeType i = 0; i < graph.nodeCount(); ++i) {
        for (auto j : graph.nextNodes(i)) {
            func(i, j);
        }
    }
}


template<class NodeType>
Graph<true, NodeType> Reverse(const Graph<true, NodeType>& g) {
    Graph<true, NodeType> reversed;
    reversed.nextNodes_.resize(g.nodeCount());
    for (int i = 0; i < g.nodeCount(); ++i) {
        for (auto j : g.nextNodes(i)) {
            reversed.nextNodes_[j].push_back(i);
        }
    }
    return reversed;
}

}

//AdjacencyList Graph::edgesToAdjacencyList(const vector<Edge>& edges, size_t nodeCount) {
//    AdjacencyList adjList(nodeCount);
//    for (const Edge& p : edges) {
//        adjList[p.first].push_back(p.second);
//        adjList[p.second].push_back(p.first);
//    }
//    return adjList;
//}
