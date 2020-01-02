
#pragma once

#include <ant/core/core.hpp>


namespace ant {
    
namespace graph {


template<class NodeType, class EdgeType>
class DirEdgedGraphBuilder;

template<class NodeType, class EdgeType>
class UndirEdgedGraphBuilder;

template<class NodeType>
class DirGraphBuilder;

template<class NodeType>
class UndirGraphBuilder;

// we are providing something like interface

// this class is going to be used both, by directed graphs and undirected graphs.
// the difference is only in how you build it.
template<class T>
class Graph {
public:
    // somewhere should be typename
    using NodeType = T;
    
protected:
	std::vector<std::vector<NodeType>> nextNodes_;
    
public:
	const std::vector<NodeType>& nextNodes(NodeType n) const {
		return nextNodes_[n];
	}
	
	Count nodeCount() const {
		return nextNodes_.size();
	}
        
    Graph<T> reversed() const {
        Graph<T> rG;
        rG.nextNodes_.resize(nodeCount());
        for (int i = 0; i < nodeCount(); ++i) {
            for (auto j : nextNodes(i)) {
                rG.nextNodes_[j].push_back(i);
            }
        }
        return rG;
    }

    friend class DirGraphBuilder<NodeType>;
    friend class UndirGraphBuilder<NodeType>;

    template<class NodeType, class EdgeType>
    friend class DirEdgedGraphBuilder; 
    template<class NodeType, class EdgeType>
    friend class UndirEdgedGraphBuilder;

};

template<class T, class E>
class EdgedGraph : public Graph<T> {
public:

    using NodeType = T;
	using EdgeType = E;

    using Graph<T>::nextNodes;
    using Graph<T>::nodeCount;
    
protected:    
	std::vector<std::vector<EdgeType>> nextEdges_;
	int edgeCount_;

public:    
	const std::vector<EdgeType>& nextEdges(NodeType n) const {
		return nextEdges_[n];
	}
	
    Count edgeCount() const {
        return edgeCount_;
    }
    
    EdgedGraph<T, E> reversed() const {
        EdgedGraph<T, E> rEG;
        rEG.nextNodes_.resize(nodeCount());
        rEG.nextEdges_.resize(nodeCount());
        for (int i = 0; i < nodeCount(); ++i) {
            for (auto p : nextPairs(i)) {
                rEG.nextNodes_[p.node].push_back(i);
                rEG.nextEdges_[p.node].push_back(p.edge);
            }
        }
        return rEG;
    }
    
	struct Pair {
		NodeType node;
		EdgeType edge;
	};
	
	using V_IT = typename std::vector<NodeType>::const_iterator;
    using D_IT = typename std::vector<EdgeType>::const_iterator;
	 
    struct Iterator : public std::iterator<std::random_access_iterator_tag, Pair> {
        
        Iterator(V_IT vIt, D_IT dIt) 
            : vIt(vIt), dIt(dIt) {}
        
        
        
        bool operator<(const Iterator it) const {
            return vIt < it.vIt;
        }
        
        bool operator!=(const Iterator it) const {
            return vIt != it.vIt;
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
        S(const std::vector<NodeType>& vs, const std::vector<EdgeType>& ds)
            : vs(&vs), ds(&ds) {}
        
        Iterator begin() const {
            return Iterator(vs->begin(), ds->begin());
        }
        
        Iterator end() const {
            return Iterator(vs->end(), ds->end());
        }
        
        const std::vector<NodeType>* vs;
        const std::vector<EdgeType>* ds;
    };
	
	S nextPairs(NodeType i) const {
		return S(nextNodes(i), nextEdges(i));
	}
    
    friend class DirEdgedGraphBuilder<NodeType, EdgeType>; 
    friend class UndirEdgedGraphBuilder<NodeType, EdgeType>;
};

template<class NodeType, class EdgeType>
class DirEdgedGraphBuilder {
protected:
	Index newEdge = 0;
	EdgedGraph<NodeType, EdgeType> g_;

public:
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
		g_.edgeCount_ = newEdge;
        return std::move(g_);
	}
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

	void Add(const std::vector<std::array<NodeType, 2>>& edges) {
        for (auto& e : edges) {
            add(e[0], e[1]);
        }
	}

	Graph<NodeType> build() {
		return std::move(g_);
	}
	
	friend class Graph<NodeType>;
};

template<class NodeType, class EdgeType>
class UndirEdgedGraphBuilder : DirEdgedGraphBuilder<NodeType, EdgeType> {

    using DirEdgedGraphBuilder<NodeType, EdgeType>::g_;
    using DirEdgedGraphBuilder<NodeType, EdgeType>::newEdge;
public:
    
    using DirEdgedGraphBuilder<NodeType, EdgeType>::build;
    using DirEdgedGraphBuilder<NodeType, EdgeType>::DirEdgedGraphBuilder;
	// use constructor of base class
	
	virtual EdgeType add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextNodes_[to].push_back(from);
		g_.nextEdges_[from].push_back(newEdge);
		g_.nextEdges_[to].push_back(newEdge);
		return newEdge++;
	}

	friend class Graph<NodeType>;
	friend class EdgedGraph<NodeType, EdgeType>;
};

template<class NodeType>
struct UndirGraphBuilder : DirGraphBuilder<NodeType> {
	
    using DirGraphBuilder<NodeType>::g_;

    using DirGraphBuilder<NodeType>::build;
    using DirGraphBuilder<NodeType>::DirGraphBuilder;

	virtual void add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextNodes_[to].push_back(from);
	}
	
	friend class Graph<NodeType>;
};


struct UndirGraphUtil {

    template<class Graph, class Handler>
    static void forEachEdge(const Graph& g, Handler handler) {
        for (auto i = 0; i < g.nodeCount(); ++i) {
            for (auto j : g.nextNode(i)) {
                if (i < j) {
                    handler(i, j);
                }
            }
        }
    }

    template<class EdgedGraph, class Handler>
    static void forEachIndexedEdge(const EdgedGraph& g, Handler handler) {
        for (auto i = 0; i < g.nodeCount(); ++i) {
            for (auto p : g.nextPairs(i)) {
                if (i < p.node) {
                    handler(i, p.node, p.edge);
                }
            }
        }
    }

};


struct DirGraphUtil {
    
    template<class Graph, class Handler>
    static void forEachEdge(const Graph& g, Handler handler) {
        for (auto i = 0; i < g.nodeCount(); ++i) {
            for (auto j : g.nextNode(i)) {
                handler(i, j);
            }
        }
    }
    
    template<class EdgedGraph, class Handler>
    static void forEachIndexedEdge(const EdgedGraph& g, Handler handler) {
        for (auto i = 0; i < g.nodeCount(); ++i) {
            for (auto p : g.nextPairs(i)) {
                handler(i, p.node, p.edge);
            }
        }
    }
    
};


template<class Value>
struct ValueEdgedGraph : EdgedGraph<Index, Index> {
    
    using EG = EdgedGraph<Index, Index>;
    
    struct Pair {
        Index node;
        Value edgeValue;
       
    };
    
    struct Iterator : public std::iterator<std::random_access_iterator_tag, Pair> {
        
        Iterator(EG::Iterator egIt, const std::vector<Value>& vals) 
            : egIt(egIt), vals(vals) {}
        
        bool operator<(const Iterator it) const {
            return egIt < it.egIt;
        }
        
        bool operator!=(const Iterator it) const {
            return egIt != it.egIt;
        }
        
        Iterator& operator+=(Count count) {
            egIt += count;
        }
        
        Iterator& operator-=(Count count) {
            egIt -= count;
        }  
        
        Iterator& operator++() {
            ++egIt;
            return *this; 
        }
        
        Pair operator*() {
            return {*egIt.vIt, vals[*egIt.dIt]};
        }
        
    private:
        EG::Iterator egIt;
        const std::vector<Value>& vals;
    };
    
    struct T {
        T(const S& s, const std::vector<Value>& vals)
        : s(s), vals(vals) {}
        
        Iterator begin() {
            return Iterator(s.begin(), vals);
        }
        
        Iterator end() {
            return Iterator(s.end(), vals);
        }
        
        const S& s;
        const std::vector<Value>& vals;
    
    };
    
    
    T nextPairs(Index i) {
        return T(EG::nextPairs(i), vals_);
    }
    
private:

    std::vector<Value> vals_;
    
};



}

}
