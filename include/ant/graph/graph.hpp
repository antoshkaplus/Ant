

#ifndef __ANT__GRAPH_H__
#define __ANT__GRAPH_H__


#include <algorithm>
#include <queue>
#include <memory>
#include <array>

#include "ant/core/core.hpp"


namespace ant {
    
namespace graph {

// this is template because we don't want to use extra memory if there would be
// small number of vertices, but graph is dense, or we need to keep many of them
template<class T>
ausing NodeAdjacencyList = AdjacencyList<Index>;
using Edge = std::array<Index, 2>;

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


//template<class Data, class AdjacencyListPtr> 
//class DataGraph_2 : Graph<AdjacencyListPtr> {
//    
//    const AdjacencyList<Data> data_adjacency_list_;
//    
//    DataGraph_2(AdjacencyListPtr adj_list, vector<Edge, Data>& data) {
//        for ()
//        
//        
//        
//    }
//    
//        
//    
//
//};


// could i have normal comparator instead of this shit??
// edge should be like a pointer... so we probably need to create our own data structure .. or... use what we have
// but have struct like (*Data, i, j)
template<class Data> 
std::vector<Edge> Kruscals(const std::vector<Edge>& edges, const std::vector<Data>& data, std::function<bool(Data&,Data&)> func) {
    auto comp = [&](Index i_0, Index i_1) {
        return func(data[i_0], data[i_1]);
    };
    auto sz = edges.size();
    Range<> r(0, sz);
    priority_queue<Index> queue(r.begin(), r.end(), comp);
    DisjointSet ds(sz);
    
    std::vector<Edge> mst;
    while (ds.size() != 1) {
        auto& e = edges[queue.top()];
        queue.pop();
        if (ds.is_separate(e[0], e[1])) {
            mst.push_back(e);
            // better would be join 
            ds.Unite(e[0], e[1]);
        }
    }
    return mst;
}



// TODO IMPLEMENT BINARY HEAP FIRST
// returns mst 
template<class Data, class AdjacencyListPtr>
void Prims(const DataGraph<Data, AdjacencyListPtr>& graph, std::function<bool(Data&,Data&)> func) {
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
    
template<class AdjacencyListPtr>
std::pair<std::vector<Index>, bool> TologicalSort(const Graph<AdjacencyListPtr>& gr) {
    std::vector<Index> L;
    std::vector<Index> S;
    std::vector<Count> incoming_count(gr.node_count(), 0);
    for (Index n = 0; n < gr.node_count(); ++n) {
        for (auto m : gr.adjacent(n)) {
            ++incoming_count[m];
        }
    }
    for (Index n = 0; n < gr.node_count(); ++n) {
        if (incoming_count[n] == 0) S.push_back(n);
    }
    while (!S.empty()) {
        Index n = S.back();
        S.pop_back();
        L.push_back(n);
        for (auto m : gr.adjacent(n)) {
            if (--incoming_count[m] == 0) {
                S.push_back(m);
            }
        }
    }
    if (std::accumulate(incoming_count.begin(), incoming_count.end(), 0) > 0) {
        return {L, false};
    }
    return {L, true};
}



bool DFS(const AdjacencyList& adj_list, int n) {
    int V = adj_list.size();
    std::vector<bool> vis(V, false);
    vis[n] = true;
    int vis_count = 1;
    std::stack<int> st;
    while (!st.empty() && vis_count != V) {
        int t = st.top();
        st.pop();
        if (!visited[t]) {
            visited[t] = true;
            ++vis_count;
            for (int a : adj_list[t]) {
                if (!visited[t]) st.push(a);
            }
        }
    } 
    return vis_count == V;
}

AdjacencyList Reverse(const AdjacencyList& adj) {
    int V = adj.size();
    AdjacencyList adj_new(V);
    for (int i = 0; i < V; ++i) {
        for (int j : adj[i]) {
            adj_new[j].push_back(i);
        }
    }
    return adj_new;
}


// for directed graph

// can implement is connected similar way

// would need to reverse
bool HasEulerianCycle(const AdjacencyList adj_list) {
    int V = adj_list.size();
    std::vector<bool> visited(V);
    for (int i = 0; i < V; ++i) {
        visited[i] = false;
    }
    // our starting vertex
    int n;
    for (int n = 0; n < V; ++n) {
        if (adj_list[n].size() > 0) {
            break;
        }
    }
    
    bool vis = DFS(adj_list, n);
    if (!vis) return false;
    
    adj_list_2 = Reverse(adj_list);
    for (int i = 0; i < V; ++i) {
        if (adj_list_2[i] != adj_list[i]) {
            return false;
        }
    }
    
    bool vis = DFS(adj_list_2, n);
    return vis;
}






} // end namespace graph

} // end namespace ant


#endif