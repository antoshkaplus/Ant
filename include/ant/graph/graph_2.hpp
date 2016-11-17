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


// when we talk about Floyd Warshall we dont consider edges at all ???

// we have to know what dist returns
template<class Value>
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
	// we use word directed to hit what we mean
    void AddDirectedDist(int from, int to, Value val) {
        dist_(from, to) = val;
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
                
				// check for max here maybe???
				//if (dist_)
				for (j = 0; j < V; j++) {
                    // If vertex k is on the shortest path from
                    // i to j, then update the value of dist[i][j]
					
					// check for max value on both before comparison???
                    if (dist_(i, k) + dist_(k, j) < dist_(i, j)) {
                        dist_(i, j) = dist_(i, k) + dist_(k, j);
                    }
                }
            }
        }
    }

}; 



// when adding edges ... this one is important
enum class GraphDirection {
    Directed,
    Undirected
};

enum class GraphConnectivity {
    Sparse,
    Dense
};

// but this thing only gives you if the edge there or not

// short vs int
// Node type

// same stuff
// Edge type

// should edges be included or not?


// how to put inside edges vs no edges
// the idea is that each edge or node can have information
// for dense thing
class GraphDenseDirected {
    Grid<EdgeType> edges;
};
// but what we really want is probably keep them together as one item
// this should create a lot of code duplication and we lose some functionality


// we can always use functions instead of methods !!!!
// why functions are so good?
// you don't have subclass to add new functionality. 
// it's easy to customize. 
// but still basic interface is needed

// maybe have object builder that would be friend to our usual classes but would call right
// things for each one of them

template<GraphConnectivity conn, class NodeType>
void Add(Graph<GraphDirection::Directed, conn, NodeType>& gr, int from, int to) {
    gr.add_directed_edge(from, to);
}

template<GraphConnectivity conn, class NodeType>
void Add(Graph<GraphDirection::Undirected, conn, NodeType>& gr, int from, int to) {
    gr.add_directed_edge(from, to);
    gr.add_directed_edge(to, from);
}

// in case of undirected graph you have two edges to return ? or only on... in both cases there is only one edge id.
// we have to become friends and manipulate edge id ourselves

// so we actually return id of the edge
template<GraphConnectivity conn, class NodeType, class EdgeType>
EdgeType Add(EdgedGraph<GraphDirection::Directed, conn, NodeType, EdgeType>& gr, int from, int to) {
	gr.add_directed_edge(from, to);
}

// can we like reduce number of items over here?
template<GraphConnectivity conn, class NodeType, class EdgeType>
EdgeType Add(EdgedGraph<GraphDirection::Undirected, conn, NodeType, EdgeType>& gr, int from, int to) {
	auto edge = gr.get_new_edge();
	gr.add_directed_edge(from, to, edge);
	gr.add_directed_edge(to, from, edge);
	return edge;
}


// look for -1?
NextNode()

NextEdge()

// so we add two more methods those would take care adding stuff to EdgedGraphs
// Add method should not be accessible. 

// it may be useful to close implementation in that case

// you go and specify protected inheritance or ....

// yes we should do it another way


template<GraphDirection direction, GraphConnectivity conn, class NodeType=int>
class Graph {};

template<GraphDirection direction, class NodeType=int>
class Graph<direction, GraphConnectivity::Dense> {
	Grid<int> mat;
    
    int node_count()

    void add_directed_edge(int from, int to) {
		mat(from, to) = 1;
	}
	
	// has to be in form of iterator 
};

template<GraphDirection direction, class NodeType=int>
class Graph<direction, GraphConnectivity::Sparse> {
	vector<vector<T>> adj_list;

    int node_count() {}

    void add_directed_edge(int from, int to) {
		adj_list[from].push_back(to);
	}
	
	const vector<NodeType>& AdjNodes(NodeType i) {
		return adj_list[i];
	}
};


template<GraphDirection dir, GraphConnectivity conn, class NodeType=int, class EdgeType=int>
// with same parameters
class EdgedGraph : Graph<dir, conn, NodeType> {
    
    // you would have to overwrite some methods
    
    
};

// have to do with many friends powers

template<GraphDirection dir, class NodeType=int, class EdgeType=int>
class EdgedGraph : Graph<dir, GraphConnectivity::Dense, NodeType> {
    // we consider from one to another
    Grid<EdgeType> mat;
    
	void add_directed_edge(int from, int to, int edge) {
		add_directed_edge(from, to);
		mat(from, to) = edge;
	}
	
	
	// we have to implement iterator inside
	// can be nodes or edges or both
	struct Adj {
		Adj(EdgedGraph& gr, int i) {
		}
		
		// can return NULL if end
		NodeType* begin() {}
		NodeType* end() {}
		
		// it doesn't quite work on matrices but can work actually
		int node;
		operator++() {
			for (auto n = node+1; n < node_count(); ++n) {
				if (mat(i, n)) {
					node = n;
					return &node;
				}
			}
			return NULL;
			
		}
	};
};

template<GraphDirection dir, class NodeType=int, class EdgeType=int>
class EdgedGraph : Graph<dir, GraphConnectivity::Sparse, NodeType> {
    // we consider from one to another
    vector<vector<Edges>> adj_list;
    // add is here
	
	// this method also could go outside
	void add_directed_edge(int from, int to, int edge) {
		add_directed_edge(from, to);
		adj_list[from].push_back(to);
	}
	
	// should return corresponding data structure with 
	// members node, edge
	AdjNodesEdges()
};

// we probably can implement remove edge too. and a lot of other stuff


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



 BFS()





