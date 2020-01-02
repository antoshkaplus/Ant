

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
using AdjacencyList = std::vector<std::vector<T>>;

using NodeAdjacencyList = AdjacencyList<Index>;
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
    
    Count CountEdges() const {
        Count c = 0;
        for (auto i = 0; i < node_count(); ++i) {
            c += adjacent(i).size();
        }
        return c;
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
    
    // and now we have to do a bunch of definitions... it sucks pretty bad
    
    
};

// with this bullshit you hide nothing
// everything is exposed.
// there is no OOP programming with this shit
Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<const NodeAdjacencyList>& ptr);
Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<NodeAdjacencyList>& ptr);

Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList* ptr);
Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList& ptr);


// if you remove edge you may mess up the whole graph

class GraphBuilder {
    
    //int node_count_;
    NodeAdjacencyList adj_list_;
    
    GraphBuilder(int node_count) 
        : adj_list_(node_count) {} //node_count_(node_count), adj_list_(node_count) {}
    
    void AddEdge(int i_1, int i_2) {
        adj_list_[i_1].push_back(i_2);
        adj_list_[i_2].push_back(i_1);
    }
    
    Graph<std::shared_ptr<const NodeAdjacencyList>> Build() {
        return Graph<std::shared_ptr<const NodeAdjacencyList>>(std::make_shared<const NodeAdjacencyList>(std::move(adj_list_)));
    }
    
};




template<class Data, class AdjacencyListPtr>
struct DataGraph : Graph<AdjacencyListPtr> {
    
    const std::shared_ptr<AdjacencyList<Data>> data_adjacency_list_;
    
    DataGraph(AdjacencyListPtr adj_list, std::shared_ptr<AdjacencyList<Data>> data_adj_list)
        : Graph<AdjacencyListPtr>(adj_list), data_adjacency_list_(data_adj_list) {}
        
    const std::vector<Data>& adjacentData(Index i) const {
        return (*data_adjacency_list_)[i];
    }     
    
    using V_IT = std::vector<Index>::iterator;
    using D_IT = typename std::vector<Data>::iterator;
        
    struct Iterator : public std::iterator<std::random_access_iterator_tag, std::tuple<Index, Data>> {
        
        Iterator(V_IT vIt, D_IT dIt) 
            : vIt(vIt), dIt(dIt) {}
        
        bool operator<(const Iterator it) {
            return vIt < it->vIt;
        }
        
        Iterator& operator+=(Count count) {
            vIt += count;
            dIt += count;
        }
        
        Iterator& operator-=(Count count) {
            vIt -= count;
            dIt -= count;
        }  
        
        Iterator& operator++() {
            ++vIt;
            ++dIt;
            return *this; 
        }
        
        std::tuple<Index, Data> operator*() {
            return {*vIt, *dIt};
        }
        
    private:
        V_IT vIt;
        D_IT dIt;
    };
    
    
    struct S {
        S(std::vector<Index>& vs, std::vector<Data>& ds)
            : vs(&vs), ds(&ds) {}
        
        Iterator begin() {
            return Iterator(vs->begin(), ds->begin());
        }
        
        Iterator end() {
            return Iterator(vs->end(), ds->end());
        }
        
        std::vector<Index>* vs;
        std::vector<Data>* ds;
    };
    
    S adjacentTuple() {
        return S(*this->adjacency_list_, *data_adjacency_list_);
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
    std::priority_queue<Index> queue(r.begin(), r.end(), comp);
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



template<class AdjacencyListPtr>
std::pair<std::vector<Index>, bool> TopologicalSort(const Graph<AdjacencyListPtr>& gr) {
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


template<class Index>
bool DFS(const AdjacencyList<Index>& adj_list, int n) {
    int V = adj_list.size();
    std::vector<bool> vis(V, false);
    vis[n] = true;
    int vis_count = 1;
    std::stack<int> st;
    while (!st.empty() && vis_count != V) {
        int t = st.top();
        st.pop();
        if (!vis[t]) {
            vis[t] = true;
            ++vis_count;
            for (int a : adj_list[t]) {
                if (!vis[t]) st.push(a);
            }
        }
    } 
    return vis_count == V;
}

template<class Index>
AdjacencyList<Index> Reverse(const AdjacencyList<Index>& adj) {
    int V = adj.size();
    AdjacencyList<Index> adj_new(V);
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
template<class Index>
bool HasEulerianCycle(const AdjacencyList<Index> adj_list) {
    int V = adj_list.size();
    std::vector<bool> visited(V);
    for (int i = 0; i < V; ++i) {
        visited[i] = false;
    }
    // our starting vertex
    int n = 0;
    for (; n < V; ++n) {
        if (adj_list[n].size() > 0) {
            break;
        }
    }
    
    bool vis = DFS(adj_list, n);
    if (!vis) return false;
    
    auto adj_list_2 = Reverse(adj_list);
    for (int i = 0; i < V; ++i) {
        if (adj_list_2[i] != adj_list[i]) {
            return false;
        }
    }
    
    vis = DFS(adj_list_2, n);
    return vis;
}
    




} // end namespace graph

} // end namespace ant


#endif