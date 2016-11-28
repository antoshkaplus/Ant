
#pragma once

#include <ant/core/core.hpp>


namespace ant {
    
namespace graph {


template<class NodeType, class EdgeType>
class DirEdgedGraphBuilder;

template<class NodeType, class EdgeType>
class UndirEdgedGraphBuilder;

// we are providing something like interface

// this class is going to be used both, by directed graphs and undirected graphs.
// the difference is only in how you build it.
template<class T>
class Graph {
public:
    // somewhere should be typename
    using NodeType = T;
    
private:
	std::vector<std::vector<NodeType>> nextNodes_;
    
public:
	const std::vector<NodeType>& nextNodes(NodeType n) const {
		return nextNodes_[n];
	}
	
	Count nodeCount() const {
		return nextNodes_.size();
	}
    
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

private:    
	std::vector<std::vector<EdgeType>> nextEdges_;
	int edgeCount_;

public:    
	const std::vector<EdgeType>& nextEdges(NodeType n) const {
		return nextEdges_[n];
	}
	
    Count edgeCount() const {
        return edgeCount_;
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
        
        Iterator begin() {
            return Iterator(vs->begin(), ds->begin());
        }
        
        Iterator end() {
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
    
	virtual void add(NodeType from, NodeType to) {
		g_.nextNodes_[from].push_back(to);
		g_.nextNodes_[to].push_back(from);
	}
	
	friend class Graph<NodeType>;
};


}

}
