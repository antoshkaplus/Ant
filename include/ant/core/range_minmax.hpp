//
//  range_minmax.hpp
//  Ant
//
//  Created by Anton Logunov on 2/20/16.
//
//

#pragma once

#include "ant/core/core.hpp"

namespace ant {


template<class T>
class RangeMinimum {
public:
    
    // n = 2**(h+1) -1 
    
    // tree consisting only from root node has zero height
    // leaf nodes have zero height

    
    RangeMinimum(const std::vector<T>& vs) : values_(vs) {
        leaf_count_ = vs.size();
        perfect_leaf_count_ = perfect_leafs(leaf_count_);
        Count nodes_c = perfect_nodes(perfect_leaf_count_) - perfect_leaf_count_ + leaf_count_;
        tree_.resize(nodes_c, -1);
        for (Index i = 0; i < leaf_count_; ++i) {
            tree_[i + nodes_c - leaf_count_] = i;
        }   
        // parent i/2     
        for (Index i = nodes_c-1; i > 0; --i) {
            // parent
            Index p = (i-1)/2;
            if (tree_[p] == -1) tree_[p] = tree_[i];
            else if (values_[tree_[p]] > values_[tree_[i]]) {
                tree_[p] = tree_[i];
            }
        }
    }
    
    // could also return corresponding Index
    const T& Minimum(Index i, Count n) {
        return Minimum(0, i, n, 0, leaf_count_, perfect_leaf_count_);
    }
    
    void update(Index i, T val) {
        
    }
    
    void rangeUpdate() {}
    
private:
    
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
    
    
    const T& Minimum(Index q, Index i, Count n_i, Index m, Count n_m, Count n_t) {
        if (i == m && n_i == n_m) {
            return node_value(q);
        }
        Count s = n_t / 2;
        // everything on the left
        if (i + n_i <= m + s) {
            return Minimum(left_child(q), i, n_i, m, std::min(s, n_m), s);
        }
        // everything on the right
        if (i >= m + s) {
            return Minimum(right_child(q), i, n_i, m+s, n_m-s, s);
        }
        // first left, last right
        const T* t = &MinimumLeft(left_child(q), i, m, std::min(s, n_m), s);
        const T* t_2 = &MinimumRight(right_child(q), i+n_i-1, m+s, n_m-s, s);
        if (*t_2 < *t) t = t_2; 
        
        return *t;
    }
    
    // left subtree
    const T& MinimumLeft(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m) {
            return node_value(q);
        }
        Count s = n_t / 2;
        if (i < m + s) {
            return std::min(MinimumLeft(left_child(q), i, m, std::min(n_m, s), s),
                            node_value(right_child(q)));
        } // else
        return MinimumLeft(right_child(q), i, m+s, n_m-s, s);
    }
    
    // right subtree
    const T& MinimumRight(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m + n_m - 1) {
            return node_value(q);
        }
        Count s = n_t / 2;
        if (i < m + s) {
            return MinimumRight(left_child(q), i, m, std::min(n_m, s), s);
        }
        return std::min(node_value(left_child(q)),
                        MinimumRight(right_child(q), i, m+s, n_m - s, s));
    }
    
    // children: 2*i, 2*i+1 +1 because started with 0
    Index left_child(Index parent) {
        return 2*parent + 1;
    }
    
    Index right_child(Index parent) {
        return 2*parent + 2;
    }
    
    const T& node_value(Index node) {
        return values_[tree_[node]];
    }
    
    Count perfect_leaf_count_;
    Count leaf_count_;
    const std::vector<T>& values_;
    std::vector<Index> tree_; 
    
};


}










