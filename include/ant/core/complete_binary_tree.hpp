//
//  complete_binary_tree.h
//  Ant
//
//  Created by Anton Logunov on 8/14/16.
//
//

#pragma once

#include "ant/core/core.hpp"

namespace ant {

    
    
template<class T>
class CompleteBinaryTree  {
    
public:
    
    CompleteBinaryTree(int leaf_count) {
        int perf_leafs = perfect_leafs(leaf_count);
        int nodes_count = perfect_nodes(perf_leafs) - perf_leafs + leaf_count;
        nodes_.resize(nodes_count);
    }
    
    
    Index left_child(Index parent) {
        Index ch = 2*parent + 1;
        assert(is_child(ch));
        return ch; 
    }
    
    Index right_child(Index parent) {
        Index ch = 2*parent + 2;
        assert(is_child(ch));
        return ch;
    }
    
    bool is_child(Index node) {
        return node < nodes_.size();
    } 
    
    T& node(Index node) {
        return nodes_[node];
    }
    
    const T& node(Index node) const {
        return nodes_[node];
    }
    
    template<class Func>
    void ForEachNode(Func func) {
        for (auto& n : nodes_) {
            func(n);
        }
    }
    
    static Count perfect_leafs(Count leaf_count) {
        // nearest power of 2 for leafs in perfect tree
        Count p = std::ceil(log2(leaf_count));
        return pow_2(p);
    }
    
    static Count perfect_nodes(Count perf_leaf_count) {
        return 2*perf_leaf_count -1;
    }    
    
private:
    
    std::vector<T> nodes_;
    
};



}