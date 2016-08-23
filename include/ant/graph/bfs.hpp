//
//  bfs.hpp
//  Ant
//
//  Created by Anton Logunov on 8/20/16.
//
//

#pragma once


#include "ant/graph/graph.hpp"


namespace ant {
    
namespace graph {




template<class Process, class AdjacencyListPtr>
void BFS(const Graph<AdjacencyListPtr>& gr, vector<Index> vs, Process& pr) {
    std::queue<Index> q;
    Count c = gr.node_count();
    std::vector<bool> visited(c, false);
    for (auto v : vs) {
        visited[v] = true;
        q.push(v);
    }
    while (!q.empty()) {
        Index v = q.front();
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


//// process should take in parent
//// consider that some vertices are on the same level and we know it
//template<class Process, class AdjacencyListPtr>
//void BFS_New(const Graph<AdjacencyListPtr>& gr, vector<Index> vs, Process& pr) {
//    std::queue<BFS_P> cur;
//    std::queue<BFS_P> next;
//    
//    Count c = gr.node_count();
//    std::vector<bool> visited(c, false);
//    const int BASE = -1;
//    for (auto v : vs) {
//        visited[v] = true;
//        cur.emplace(v, BASE);
//    }
//    while (!cur.empty()) {
//        BFS_P p = cur.front();
//        cur.pop();
//        BFS_Control b = pr(p.vertex, p.from);
//        if (b.stop) return;
//        if (b.revisit) {
//            cur.push(<#const value_type &__v#>)
//        }
//        for (Index w : gr.adjacent(v)) {
//            if (!visited[w]) {
//                visited[w] = true;
//                q.push(w);
//            }
//        }
//    }
//}


//    // what we can do is some kind of template with following arguments
//    template<Revisit|Stop|From>
//    // we can implement all three options and call it BFS
//    
//    // can make it a class on creation you pass variables in and and try to assign with whatever
//    
//    
//    // with this function we also provide parent in
//    template<class Process, class AdjacencyListPtr>
//    void BFS_From() {
//        
//        
//    }


template<class Process, class AdjacencyListPtr> 
void BFS_LateRevisit(const Graph<AdjacencyListPtr>& gr, vector<Index> vs, Process& pr) {
    std::queue<Index> q;
    Count c = gr.node_count();
    std::vector<bool> visited(c, false);
    for (auto v : vs) {
        visited[v] = true;
        q.push(v);
    }
    while (!q.empty()) {
        Index v = q.front();
        q.pop();
        bool b = pr(v);
        if (b) {
            q.push(v);
            continue;
        }
        for (Index w : gr.adjacent(v)) {
            if (!visited[w]) {
                visited[w] = true;
                q.push(w);
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




}

}