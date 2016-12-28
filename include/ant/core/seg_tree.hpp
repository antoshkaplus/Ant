//
//  seg_tree.hpp
//  Ant
//
//  Created by Anton Logunov on 12/27/16.
//
//

#pragma once

#include "ant/core/core.hpp"

namespace ant {

// Op is callable. args: two items
template<class T, class Op>
class SegTree {
public:
    
    // n = 2**(h+1) -1 
    
    // tree consisting only from root node has zero height
    // leaf nodes have zero height

    SegTree(const std::vector<T>& vs, Op op) : op_(op) {
        leaf_count_ = vs.size();
        perfect_leaf_count_ = perfect_leafs(leaf_count_);
        Count nodes_c = perfect_nodes(perfect_leaf_count_) - perfect_leaf_count_ + leaf_count_;
        values_.resize(nodes_c);
		std::vector<bool> inited(nodes_c, false);
        for (Index i = 0; i < leaf_count_; ++i) {
            values_[i + nodes_c - leaf_count_] = vs[i];
			inited[i + nodes_c - leaf_count_] = true;
        }   
        // parent i/2     
        for (Index i = nodes_c-1; i > 0; --i) {
            Index p = (i-1)/2;
            // now we have to merge
            if (!inited[p]) {
                values_[p] = values_[i];
                inited[p] = true;
            } else { 
                // now we have to merge
                // was initialized with right child
                values_[p] = op_(values_[i], values_[p]); 
            }
        }
    }
    
    // could also return corresponding Index
    T Query(Index i, Index j) {
        return Query(0, i, j-i+1, 0, leaf_count_, perfect_leaf_count_);
    }
    
    void update(Index i, T val) {
        Update(0, i, perfect_leaf_count_, val);
    }
    
private:
    
    void Update(Index q, Index i, Count n_t, T val) {
        if (n_t == 1) {
            values_[q] = val;
            return;
        } 
        auto s = n_t / 2;
        if (i < s) {
            Update(left_child(q), i, s, val);
        } else {
            Update(right_child(q), i-s, s, val);
        }
        // interval may not be there at all
        if (right_child(q) < values_.size()) {
            values_[q] = op_(node_value(left_child(q)), node_value(right_child(q)));
        } else {
            values_[q] = node_value(left_child(q));
        }
        
    }
	
    Count perfect_leafs(Count leaf_count) {
        // nearest power of 2 for leafs in perfect tree
        Count p = std::ceil(log2(leaf_count));
        return std::pow(2, p);
    }
    
    Count perfect_nodes(Count perf_leaf_count) {
        return 2*perf_leaf_count -1;
    }    
    
    // m = how many elements we have
    // t = how many elements for perfect tree
    
    // m = start what we consider right now in leaf
    // n_t = perfect tree leafs
    // n_m = current leafs
    
    // q = parent (current node)
    // i = left most value (leaf) index
    // n_i = how many elements consider
    
    
    T Query(Index q, Index i, Count n_i, Index m, Count n_m, Count n_t) {
        if (i == m && n_i == n_m) {
            return node_value(q);
        }
        Count s = n_t / 2;
        // everything on the left
        if (i + n_i <= m + s) {
            return Query(left_child(q), i, n_i, m, std::min(s, n_m), s);
        }
        // everything on the right
        if (i >= m + s) {
            return Query(right_child(q), i, n_i, m+s, n_m-s, s);
        }
        // first left, last right
        auto t = QueryLeft(left_child(q), i, m, std::min(s, n_m), s);
        auto t_2 = QueryRight(right_child(q), i+n_i-1, m+s, n_m-s, s);
        return op_(t, t_2);
    }
    
    // left subtree
    T QueryLeft(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m) {
            return node_value(q);
        }
        Count s = n_t / 2;
        if (i < m + s) {
            return op_(QueryLeft(left_child(q), i, m, std::min(n_m, s), s),
                       node_value(right_child(q)));
        } // else
        return QueryLeft(right_child(q), i, m+s, n_m-s, s);
    }
    
    // right subtree
    T QueryRight(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m + n_m - 1) {
            return node_value(q);
        }
        Count s = n_t / 2;
        if (i < m + s) {
            return QueryRight(left_child(q), i, m, std::min(n_m, s), s);
        }
        return op_(node_value(left_child(q)),
                   QueryRight(right_child(q), i, m+s, n_m - s, s));
    }
    
    // children: 2*i, 2*i+1 +1 because started with 0
    Index left_child(Index parent) {
        return 2*parent + 1;
    }
    
    Index right_child(Index parent) {
        return 2*parent + 2;
    }
    
    const T& node_value(Index node) {
        return values_[node];
    }
    
    Count perfect_leaf_count_;
    Count leaf_count_;
    std::vector<T> values_;
	Op op_;
    
};


}
