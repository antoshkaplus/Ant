

#ifndef __ANT__GRAPH_H__
#define __ANT__GRAPH_H__


#include <algorithm>
#include <queue>
#include <memory>

#include "ant/core/core.hpp"


namespace ant {
    
namespace graph {

// this is template because we don't want to use extra memory if there would be
// small number of vertices, but graph is dense, or we need to keep many of them
template<class T>
using AdjacencyList = std::vector<std::vector<T>>;
using NodeAdjacencyList = AdjacencyList<Index>;
using Edge = std::pair<Index, Index>;

NodeAdjacencyList EdgesToAdjacencyList(const std::vector<Edge>& edges, size_t node_count);


// struct provides access on reading. 
// const prevents alternating variables 
// immutable class
// like wrapper
// can't use unique_ptr here or will be unable to copy
template <class NodeAdjacencyListPtr, 
    EnableIf<IsAnySame<NodeAdjacencyListPtr, std::shared_ptr<const NodeAdjacencyList>, 
                                         const NodeAdjacencyList* >> = enabler >
struct Graph {
    
    NodeAdjacencyListPtr adjacency_list_;
    
    Graph(NodeAdjacencyListPtr adj_list) : adjacency_list_(adj_list) {}  
        
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
        NodeAdjacencyList *adj_list_ptr = new NodeAdjacencyList(node_count);
        NodeAdjacencyList& adj_list = *adj_list_ptr;
        for (auto i = 0; i < node_count; ++i) {
            for (auto j = i+1; j < node_count; ++j) {
                if (distr(rng) < 0.5) continue;
                adj_list[i].push_back(j);
                adj_list[j].push_back(i);
            }
        }        
        return Graph(std::shared_ptr<NodeAdjacencyList>(adj_list_ptr));       
    }
};


Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<const NodeAdjacencyList>& ptr);
Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<NodeAdjacencyList>& ptr);

Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList* ptr);
Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList& ptr);



template<class Data, class AdjacencyListPtr>
struct DataGraph : Graph<AdjacencyListPtr> {
    
    const std::shared_ptr<AdjacencyList<Data>> data_adjacency_list_;
    
    DataGraph(AdjacencyListPtr adj_list, std::shared_ptr<AdjacencyList<Data>> data_adj_list)
        : Graph<AdjacencyListPtr>(adj_list), data_adjacency_list_(data_adj_list) {}
        
    const std::vector<Data>& adjacentData(Index i) const {
        return (*data_adjacency_list_)[i];
    }     
};




// TODO IMPLEMENT BINARY HEAP FIRST
// returns mst 
template<class Data, class AdjacencyListPtr>
void Prims(const DataGraph<Data, AdjacencyListPtr>& graph, std::function<bool(Data& d_0, Data& d_1)> func) {
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


// process can return boolean value: if true, then terminate early
template<class Process, class AdjacencyListPtr>
void BFS(const Graph<AdjacencyListPtr>& gr, Index v, Process& pr) {
    std::queue<Index> q;
    Count c = gr.node_count();
    std::vector<bool> visited(c, false);
    visited[v] = true;
    q.push(v);
    while (!q.empty()) {
        v = q.front();
        q.pop();
        // should we also pass from where we came from
        bool b = pr(v);
        if (b) return;
        for (Index w : gr.adjacent(v)) {
            if (!visited[w]) {
                visited[w] = true;
                q.push(w);
            }
        }
    }
}

enum class BFS_Flow {
    // don't expand on children
    Skip,
    Continue,
    Terminate
};


// sometimes it's important for you to have previous vertex
// but here we don't process initial vertex because it has no previous one
template<class Process, class AdjacencyListPtr>
void BFS_Prev(const Graph<AdjacencyListPtr>& gr, Index v, Process& pr) {
    std::queue<Index> q;
    Count c = gr.node_count();
    std::vector<bool> visited(c, false);
    visited[v] = true;
    q.push(v);
    while (!q.empty()) {
        v = q.front();
        q.pop();
        // should we also pass from where we came from
        for (Index w : gr.adjacent(v)) {
            if (!visited[w]) {
                BFS_Flow flow = pr(w, v);
                if (flow == BFS_Flow::Terminate) return;
                visited[w] = true;
                if (flow == BFS_Flow::Skip) continue;
                q.push(w);
            }
        }
    }
}
    




} // end namespace graph

} // end namespace ant


#endif