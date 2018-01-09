//
//  bfs_3.hpp
//  Ant
//
//  Created by Anton Logunov on 11/18/16.
//
//

#pragma once


#include "ant/graph/graph_3.hpp"


namespace ant {

namespace graph {


    
// process can return boolean value: if true, then terminate early
template<class Process, class Graph>
void BFS(const Graph& gr, typename Graph::NodeType v, Process& pr) {
    std::queue<typename Graph::NodeType> q;
    auto c = gr.node_count();
    std::vector<bool> visited(c, false);
    visited[v] = true;
    q.push(v);
    while (!q.empty()) {
        v = q.front();
        q.pop();
        bool b = pr(v);
        if (b) return;
        for (auto w : gr.nextNodes(v)) {
            if (!visited[w]) {
                visited[w] = true;
                q.push(w);
            }
        }
    }
}


// when we tell to revisit ...

struct BFS_Control {
    BFS_Control() : stop(false), revisit(false) {}
    
    bool stop;
    bool revisit;
};

struct BFS_P {
    Index vertex;
    Index from;
};


enum class BFS_Flow {
    // don't expand on children
    Skip,
    Continue,
    Terminate
};


// sometimes it's important for you to have previous vertex
// but here we don't process initial vertex because it has no previous one
template<class Process, class Graph>
void BFS_Prev(const Graph& gr, Index v, Process& pr) {
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


struct BFS {

};


template<class EdgedGraph, class Value>
struct WeightedBFS {

    WeightedBFS(const EdgedGraph& g, const std::vector<Value>& vals) 
        : g(g), vals(vals) {} 

    // this one is usual
    template<class Proc>
    void run() {
        
    }
    
    
    void runPrev(Index orig) {
        
    }
    
    // usually origins have special processing.
    // so we don't want to handle it here
    template<class Proc>
    void runPrev(const std::vector<Index>& origs, Proc pr) const {
        
        struct Item {
            Index from;
            Index to;
            Index edge;
            Value val;

            Item() {}
            Item(Index from, Index to, Index edge, Value val)
                    : from(from), to(to), edge(edge), val(val) {}

            bool operator<(const Item& item) const {
                return val > item.val;
            }
        };
        
        std::vector<bool> vis(g.nodeCount(), false);
        std::priority_queue<Item> q;
        for (auto v : origs) {
            q.emplace(-1, v, -1, 0);
        }
        while (!q.empty()) {
            auto i = q.top();
            q.pop();
            if (vis[i.to]) continue;
            
            if (i.from != -1) pr(i.to, i.from, i.edge, i.val);
            
            vis[i.to] = true;
            for (auto p : g.nextPairs(i.to)) {
                if (!vis[p.node]) {
                    q.emplace(i.to, p.node, p.edge, i.val + vals[p.edge]);
                }
            }
        }
    }

private:
    
    const EdgedGraph& g;
    const std::vector<Index>& vals;
};



// we try to Skip before visiting the node, because 
// in current version of algorithm we are about of visiting edges.
// by skipping we cancel addition of outgoing edges from second endpoint   
template<class Process, class EdgedGraph>
void BFS_PrevEdged(const EdgedGraph& gr, Index v, Process& pr) {
    std::queue<Index> q;
    Count c = gr.nodeCount();
    std::vector<bool> visited(c, false);
    visited[v] = true;
    q.push(v);
    while (!q.empty()) {
        v = q.front();
        q.pop();
        for (auto p : gr.nextPairs(v)) {
            auto w = p.node;
            if (!visited[w]) {
                BFS_Flow flow = pr(w, v, p.edge);
                if (flow == BFS_Flow::Terminate) return;
                if (flow == BFS_Flow::Skip) continue;
                visited[w] = true;
                q.push(w);
            }
        }
    }
}


// RIGHT NOW WORKS FOR TREES
// can know the whole history by using our vector
template<class AdjacencyListPtr>
int Diameter(const Graph<AdjacencyListPtr>& gr) {
    auto last = 0;
    std::vector<int> arr(gr.nodeCount());
    auto func = [&] (auto v, auto prev) {
        arr[v] = prev;
        last = v;
        return BFS_Flow::Continue;
    };
    BFS_Prev(gr, 0, func);
    arr[last] = last;
    BFS_Prev(gr, last, func);
    // now compute size
    int cur = last;
    int length = 0;
    while (arr[cur] != cur) {
        ++length; 
        cur = arr[cur];
    }
    return length;
}




}

}
