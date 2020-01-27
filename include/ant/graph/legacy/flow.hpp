//
//  flow.hpp
//  Ant
//
//  Created by Anton Logunov on 10/15/16.
//
//

#pragma once

#include "ant/graph/model/graph_3.hpp"
#include "ant/graph/bfs_3.hpp"


namespace ant {

namespace graph {    



// but BFS should be slow right???

// maximum flow in a flow network
template<class DirEdgedGraph, class Value> 
class FordFulkerson {

    
    using NodeType = typename DirEdgedGraph::NodeType;
    using EdgeType = typename DirEdgedGraph::EdgeType;
    
    
    struct Edge {
        NodeType from;
        NodeType to;
        
        // i have to copy as I'm going to manipulate it
        Value flow;
        Value capacity;
        
        Edge() {}
        
        Edge(NodeType from, NodeType to, Value flow, Value capacity) 
            : from(from), to(to), flow(flow), capacity(capacity) {}
        
        // if graph is directed then this comparison part is not really needed
        Value ResidualCapacityTo(Index vertex) {
            if      (vertex == from) return flow;            // backward edge
            else if (vertex == to) return capacity - flow;   // forward edge
            else throw std::runtime_error("Illegal endpoint");
        }
        
        void AddResidualFlowTo(Index vertex, Value delta) {
            if      (vertex == from) flow -= delta;           // backward edge
            else if (vertex == to) flow += delta;             // forward edge
            else throw std::runtime_error("Illegal endpoint");
        }

    };

    std::vector<Edge> edges_;
    EdgedGraph<NodeType, EdgeType> graph_;
    
    
    struct P {
        Index from;
        Index edge;
    };
    
public:

    FordFulkerson(const DirEdgedGraph& graph, const std::vector<Value>& edgeValues) : graph_(graph) {
        auto sz = graph.nodeCount();
        
        edges_.resize(graph.edgeCount());
        for (auto i = 0; i < sz; ++i) {
            for (auto a : graph.nextPairs(i)) {        
                edges_[a.edge] = Edge{i, a.node, 0, edgeValues[a.edge]};
            }
        }
        
        UndirEdgedGraphBuilder<NodeType, EdgeType> builder(sz);
        for (auto& e : edges_) {
            builder.add(e.from, e.to);
        }
        graph_ = builder.build();
    }

    Value Compute(Index s, Index t) {
        
        auto sz = graph_.nodeCount();
        
        std::vector<P> parent(sz); 
        parent[s].from = s;
        bool t_visited;
        
        auto pr = [&](Index v, Index from, Index edge) {
            if (edges_[edge].ResidualCapacityTo(v) > 0) {
                parent[v] = {from, edge};
                if (v == t) {
                    t_visited = true;   
                    return BFS_Flow::Terminate;
                } 
            } else {
                return BFS_Flow::Skip;
            }
            return BFS_Flow::Continue;
        };
        
        int max_flow = 0;
        
        for (;;) {
            t_visited = false;
            BFS_PrevEdged(graph_, s, pr);
            if (!t_visited) {
                break;
            }
            auto path_flow = std::numeric_limits<Value>::max();
            for (auto v = t; v != s; v = parent[v].from) {
                auto& e = edges_[parent[v].edge];
                path_flow = std::min(path_flow, e.ResidualCapacityTo(v));
            }
            
            for (auto v = t; v != s; v = parent[v].from) {
                edges_[parent[v].edge].AddResidualFlowTo(v, path_flow);
            }
            
            max_flow += path_flow;
        }
        
        return max_flow;
    }

    std::vector<Value> flow() const {
        std::vector<Value> res(edges_.size());
        auto getValue = [&](const Edge& e) { return e.flow; };
        std::transform(edges_.begin(), edges_.end(), res.begin(), getValue);
        return res;
    }

private:

    
};


class MaxBipartiteMatching {
    
    DirEdgedGraphBuilder<Index, Index> builder_;
    Index fromStart_;
    Index toStart_;
    Index src_;
    Index dst_;
    // capacities
    std::vector<Count> edgeValues_;

    struct M {
        Index from, to;
        Count count;
        
        M() {}
        M(Index from, Index to, Count count)
            : from(from), to(to), count(count) {}
    };

    Count fromSize() const {
        return toStart_ - fromStart_;
    }

public:

    MaxBipartiteMatching(std::vector<Count> from, std::vector<Count> to) : builder_(0) {
        fromStart_ = 0;
        toStart_ = from.size();
        src_ = toStart_ + to.size();
        dst_ = src_ + 1;
        builder_ = DirEdgedGraphBuilder<Index, Index>(from.size() + to.size() + 2);
        
        for (auto i = 0; i < from.size(); ++i) {
            builder_.add(src_, fromStart_+i);
            edgeValues_.push_back(from[i]);
        }
        for (auto i = 0; i < to.size(); ++i) {
            builder_.add(toStart_+i, dst_);
            edgeValues_.push_back(to[i]);
        }
    }
    
    void addDirEdge(Index from, Index to, Count val) {
        builder_.add(fromStart_+from, toStart_+to);
        edgeValues_.push_back(val);
    }
    
    // maybe do it different way
    std::vector<M> Compute() {
        auto g = builder_.build();
        FordFulkerson<decltype(g), Count> ff(g, edgeValues_);
        ff.Compute(src_, dst_);
        edgeValues_ = ff.flow();
        return matching(g);
    }
    
    std::vector<M> matching(const EdgedGraph<Index, Index>& g) {
        std::vector<M> r;
        for (auto i = 0; i < fromSize(); ++i) {
            auto from = i + fromStart_;
            for (auto p : g.nextPairs(from)) {
                if (edgeValues_[p.edge] > 0) {
                    r.emplace_back(i, p.node-toStart_, edgeValues_[p.edge]);
                }
            }
        }
        return r;
    }
};


}

}
