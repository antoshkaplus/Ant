//
//  flow.hpp
//  Ant
//
//  Created by Anton Logunov on 10/15/16.
//
//

#pragma once

#include "ant/graph/bfs.hpp"


namespace ant {

namespace graph {    



// but BFS should be slow right???
template<class Value, class AdjacencyListPtr> 
class FordFulkerson {
    
    
    struct Edge {
        Index from;
        Index to;
        
        // i have to copy as I'm going to manipulate it
        Value flow;
        Value capacity;
        
        Edge(Index from, Index to, Value flow, Value capacity) 
            : from(from), to(to), flow(flow), capacity(capacity) {}
            
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
    
    using Graph = DataGraph<Value, AdjacencyListPtr>;
    using ResidualGraph = DataGraph<Index, std::unique_ptr<AdjacencyListNode>>;
    
    std::vector<Edge> edges_;
    ResidualGraph res_graph_;
    
    struct P {
        Index from;
        Index edge;
    }
    
public:
    Value Compute(const &Graph graph, Index s, Index t) {
        int u, v;
        
        auto sz = graph.node_count();
        Prepare(graph); 
        
        std::vector<P> parent(sz); 
        parent[s].from = s;
        auto t_visited = false;
        
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
            BFS_Prev(res_graph_, s, pr);
            if (!t_visited) {
                break;
            }
            auto path_flow = MAX;
            for (auto v = t; v != s; v = parent[v].from) {
                auto& e = edges_[parent[v].edge];
                path_flow = min(path_flow, e.ResidualCapacityTo(v));
            }
            
            for (auto v = t; v != s; v = parent[v].from) {
                edges_[v].AddResidualFlowTo(v, path_flow)
            }
            
            max_flow += path_flow;
        }
        
        return max_flow;
    }

private:
    
    void Prepare(const Graph& graph) {
        auto sz = graph.node_count();
        
        edges_.reserve(graph.CountEdges());
        
        AdjacencyList<Index> adj(sz);
        AdjacencyList<Index> adj_edge(sz);
        
        auto edge_index = 0;
        for (auto i = 0; i < sz; ++i) {
            Value v;
            Index j;
            for (auto a : graph.AdjacentTuple(i)) {        
                tie(j, v) = a;
                edges_.emplace_back(i, j, 0, v);
                adj[i].push_back(j);
                adj[j].push_back(i);
                adj_edge[i].push_back(edge_index);
                adj_edge[j].push_back(edge_index);
                ++edge_index;
            }
        }
        res_graph_ = ResidualGraph(
            std::make_shared(std::move(adj)), 
            std::make_shared(std::move(adj_edge)));
    }
    
    
    
    void Process() {
       
    }
    
};


}

}
