#pragma once

#include "ant/graph/model/graph.hpp"

namespace ant::graph {

template<bool directed, class NodeType, class EdgeType=NodeType>
class EdgeGraph : public Graph<directed, NodeType> {

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

};

template<bool directed> using EdgeGraph32 = EdgeGraph<directed, Index>;
template<bool directed> using EdgeGraph16 = EdgeGraph<directed, int16_t>;

template<bool directed, class NodeType>
class EdgeGraphBuilder;

template<class NodeType, class EdgeType=NodeType>
class EdgeGraphBuilder<true, NodeType, EdgeType> {

};

template<class NodeType, class EdgeType=NodeType>
class EdgeGraphBuilder<false, NodeType, EdgeType> {

};

}