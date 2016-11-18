
// we are providing something like interface

// this class is going to be used both, by directed graphs and undirected graphs.
// the difference is only in how you build it.
template<class T>
class Graph {
	// somewhere should be typename
	using NodeType = T;
	
	vector<vector<NodeType>> nextNodes_;
	
	const vector<NodeType> nextNodes(NodeType n) const {
		return nextNodes_[n];
	}
	
	Count nodeCount() const {
		return nextNodes_.size();
	}
};

template<class T, class E>
class EdgedGraph : public Graph<T> {
	
	using EdgeType = E;
	
	vector<vector<EdgeType>> nextEdges_;
	
	const vector<EdgeType> nextEdges(NodeType n) const {
		return nextEdges_[n];
	}
	
	struct Pair {
		NodeType node;
		EdgeType edge;
	};
	
	using V_IT = typename std::vector<NodeType>::iterator;
    using D_IT = typename std::vector<EdgeType>::iterator;
    using P = Pair; 
	 
    struct Iterator : public std::iterator<std::random_access_iterator_tag, Pair> {
        
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
        
        Pair operator*() {
            return {*vIt, *dIt};
        }
        
    private:
        V_IT vIt;
        D_IT dIt;
    };
    
    // should better use const
    struct S {
        S(std::vector<NodeType>& vs, std::vector<EdgeType>& ds)
            : vs(&vs), ds(&ds) {}
        
        Iterator begin() {
            return Iterator(vs->begin(), ds->begin());
        }
        
        Iterator end() {
            return Iterator(vs->end(), ds->end());
        }
        
        std::vector<NodeType>* vs;
        std::vector<EdgeType>* ds;
    };
	
	S nextPairs() {
		return S(*this->nextNodes_, *nextEdges_);
	}
};

template<class NodeType, class EdgeType>
struct DirEdgedGraphBuilder {
	
	Index newEdge = 0;
	EdgedGraph<NodeType, EdgeType> g_;
	
	DirEdgedGraphBuilder(Count nodeCount) {
		g_.nextNodes_.resize(nodeCount);
		g_.nextEdges_.resize(nodeCount);
	}
	
	virtual EdgeType add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextEdges_[from].push_back(newEdge);
		return newEdge++;
	}
	
	EdgedGraph<NodeType, EdgeType> build() {
		return std::move(g_);
	}
	
	friend template class Graph;
	friend template class EdgedGraph;
};

template<class NodeType>
struct DirGraphBuilder {
	
	Graph<NodeType> g_;
	
	DirGraphBuilder(Count nodeCount) {
		g_.nextNodes_.resize(nodeCount);
	}
	
	virtual void add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
	}
	
	Graph<NodeType> build() {
		return std::move(g_);
	}
	
	friend template class Graph;
};

struct UndirEdgedGraphBuilder : DirEdgedGraphBuilder {
	
	// use constructor of base class
	
	virtual EdgeType add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextNodes_[to].push_back(from);
		g_.nextEdges_[from].push_back(newEdge);
		g_.nextEdges_[to].push_back(newEdge);
		return newEdge++;
	}
	
	friend template class Graph;
	friend template class EdgedGraph;
};

struct UndirGraphBuilder : DirGraphBuilder {
	
	virtual void add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextNodes_[to].push_back(from);
	}
	
	friend template class Graph;
};
