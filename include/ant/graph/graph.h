

#ifndef __ANT__GRAPH_H__
#define __ANT__GRAPH_H__


#include <algorithm>


#include "ant/core.h"


namespace ant {
    
namespace graph {

template<class T>
using  AdjacencyList = std::vector<std::vector<T>>;

using Edge = std::pair<Index, Index>;



AdjacencyList<Index> EdgesToAdjacencyList(const std::vector<Edge>& edges, size_t node_count) {
    AdjacencyList<Index> adj_list(node_count);
    for (const Edge& p : edges) {
        adj_list[p.first].push_back(p.second);
        adj_list[p.second].push_back(p.first);
    }
    return adj_list;
}


// struct provides access on reading. 
// const prevents alternating variables 
// immutable class
struct Graph {
    
    const std::shared_ptr<AdjacencyList<Index>>  adjacency_list_;
    
    Graph(std::shared_ptr<AdjacencyList<Index>> adj_list) : adjacency_list_(adj_list) {}  
        
    Count degree(Index i) const {
        return (*adjacency_list_)[i].size();
    }

    const std::vector<Index>& adjacent(Index i) const {
        return (*adjacency_list_)[i];
    }
    
    Count node_count() const {
        return adjacency_list_->size();
    }
    
    // completeness between 0 and 1 : ratio of edges
    static Graph Random(Count node_count, double completeness) {
        std::default_random_engine rng;
        std::uniform_real_distribution<> distr;
        AdjacencyList<Index> *adj_list_ptr = new AdjacencyList<Index>(node_count);
        AdjacencyList<Index> adj_list = *adj_list_ptr;
        for (auto i = 0; i < node_count; ++i) {
            for (auto j = i+1; j < node_count; ++j) {
                if (distr(rng) < 0.5) continue;
                adj_list[i].push_back(j);
                adj_list[j].push_back(i);
            }
        }        
        return Graph(std::shared_ptr<AdjacencyList<Index>>(adj_list_ptr));       
    }
};

template<class Data>
struct DataGraph : Graph {
    
    const std::shared_ptr<AdjacencyList<Data>> data_adjacency_list_;
    
    DataGraph(std::shared_ptr<AdjacencyList<Index>> adj_list, std::shared_ptr<AdjacencyList<Data>> data_adj_list)
        : Graph(adj_list), data_adjacency_list_(data_adj_list) {}
        
    const std::vector<Data>& adjacentData(Index i) const {
        return (*data_adjacency_list_)[i];
    }     
};




// TODO IMPLEMENT BINARY HEAP FIRST
// returns mst 
template<class Data>
Graph Prims(const DataGraph<Data>& graph, std::function<bool(Data& d_0, Data& d_1)> func) {
    // need to reverse to make min heap
    auto comp = [=](Index i_0, Index i_1) {
        return true;
    };
    
    struct P {
        Data& weight;
        Index other_node;
    };
    
    std::vector<Index> nodes;
    
    std::vector<P> ps;
    std::vector<bool> bs;
    
    AdjacencyList<Index> mst(nodes.size());
    for (int i = 0; i < graph.node_count(); ++i) {
        std::pop_heap(nodes.begin(), nodes.end(), comp);
        Index n = nodes.back();
        nodes.pop_back();
        Index n_2 = ps[n].other_node;
        mst[n].push_back(n_2);
        mst[n_2].push_back(n);
        
        auto& adj_nodes = graph.adjacent(n_2);
        auto& adj_weight = graph.adjacentData(n_2);
        for (auto k = 0; k < adj_nodes.size(); ++k) {
            if (func(adj_weight[k], ps[adj_nodes[k]].weight)) {
                // need to pop special element
                ps[adj_nodes[k]] = {adj_weight[k], adj_nodes[k]};
            }
        }
                
    }
    
    
    

}




} // end namespace graph

} // end namespace ant


#endif