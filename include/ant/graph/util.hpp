//
//  util.hpp
//  Ant
//
//  Created by Anton Logunov on 12/9/16.
//
//

#pragma once

#include "ant/graph/model/graph_3.hpp"


namespace ant {

namespace graph {
   
    
template<class Graph>
Graph reverseGraph {
    
};


template<class EdgedGraph>
AdjacencyList<Index> reverseEdgedGraph(const EdgedGraph<Index>& adj) {
    int V = adj.size();
    AdjacencyList<Index> adj_new(V);
    for (int i = 0; i < V; ++i) {
        for (int j : adj[i]) {
            adj_new[j].push_back(i);
        }
    }
    return adj_new;
}

      
}

}
