//
//  graph_2.hpp
//  Ant
//
//  Created by Anton Logunov on 11/4/16.
//
// We divide graphs:
// - directed/undirected
// - index type for edge/vertex
// - deleting edges and vertices
// - keep edges
// - sparse/dense
// 

#pragma once



// all shortest paths
class BellmanFord {
    
    void Init(const Graph& graph) {
        
    }
    
    void Compute() {
        
    }
    
    
    
};


// we have to know what dist returns
template<class Graph, class Value>
class FloydWarshall {
    
    Grid<Value> dist_;
    
    int node_count() const {
        return dist_.row_count();
    }    

public:
    FloydWarshall(int node_count, Value max) 
        : dist_(node_count, node_count, max){}
    
    
    
    // should use both directions or only one...
    // use type_traits to resolve
    void AddDist(int i_1, int i_2, Value val) {
        dist_(i_1, i_2) = dist_(i_2, i_1) = val;
    }
    
    // should Dist be passed in init or in compute
    
    void Compute() {
        /* 
        Add all vertices one by one to the set of intermediate vertices.
        ---> Before start of a iteration, we have shortest distances between all
        pairs of vertices such that the shortest distances consider only the
        vertices in set {0, 1, 2, .. k-1} as intermediate vertices.
        ----> After the end of a iteration, vertex no. k is added to the set of
        intermediate vertices and the set becomes {0, 1, 2, .. k} 
        */
        for (k = 0; k < V; k++) {
            // Pick all vertices as source one by one
            for (i = 0; i < V; i++) {
                // Pick all vertices as destination for the
                // above picked source
                for (j = 0; j < V; j++) {
                    // If vertex k is on the shortest path from
                    // i to j, then update the value of dist[i][j]
                    if (dist_(i, k) + dist_(k, j) < dist_(i, j)) {
                        dist_(i, j) = dist_(i, k) + dist_(k, j);
                    }
                }
            }
        }
    }

}; 



enum class GraphType {
    Directed,
    Undirected
}





// what if we want to store edges
template<class NodeType, GraphType graph_type>
class Graph {


};



// we can improve memory locality
class GraphBuilder {
    
    // it should be like inner class anyway. as we are going to iterate over it.
    struct Item {
        int node, edge;
    };

    vector<vector<Item>> adj_list;
    int new_edge_id;
    
    // returns edge id. you can store data in array and use id to refer to the edge.
    int AddEdge(int i_1, int i_2) {
        adj_list[i_1].emplace();
        adj_list[i_2].emplace();
        
        // what if user wants to know nodes by providing edge id.
        
        // it would add up additional cost and no real usage
    }


};