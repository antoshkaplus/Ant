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