//
//  range_count.hpp
//  Ant
//
//  Created by Anton Logunov on 8/14/16.
// 
//  For more information on this class please visit SPOJ problem/solution for INTERVAL - Intervals 

#pragma once


#include "ant/core/complete_binary_tree.hpp"


namespace ant {
    
    
    // this one is customized for pecial case (check add method)
    // it should look different otherwise probably
    // like when you add, you add to particular value in the tree
    // or something
    
    // add something about taken
    class RangeCount {
        
    public:
        // what about providing some default value.
        RangeCount(ant::Count elem_count)
        : elem_count_(elem_count), perf_leaf_count_(CompleteBinaryTree<int>::perfect_leafs(elem_count)), taken_(elem_count) {
            
            auto func = [](ant::Count& node) { node = 0; };
            taken_.ForEachNode(func);
        }
        
        void AddToRight(int first, int last, int how_many) {
            if (how_many <= 0) return;
            AddToRight(0, first, last, 0, elem_count_-1, how_many, perf_leaf_count_);
        }
        
        int Count(int first, int last) {
            return Count(0, first, last, 0, elem_count_-1, perf_leaf_count_);
        }
        
    private:
        
        void AddToRight(int node_index, int first, int last, int from, int to, int how_many, int perf_leaf_count) {
            assert(perf_leaf_count != 0);
            auto count = (taken_.node(node_index) += how_many);
            if (count == to - from + 1) return;
            
            int perf_leaf_half = perf_leaf_count/2;
            int first_right = from + perf_leaf_half;
            
            if (last < first_right) {
                node_index = taken_.left_child(node_index);
                to = min(first_right - 1, to);
                AddToRight(node_index, first, last, from, to, how_many, perf_leaf_half);
                return;
            }
            
            if (first >= first_right) {
                node_index = taken_.right_child(node_index);
                from = first_right;
                AddToRight(node_index, first, last, from, to, how_many, perf_leaf_half);
                return;
            }
            
            
            // count right free that's in bound
            int r_ch = taken_.right_child(node_index);    
            int r_count = Count(r_ch, first_right, last, first_right, to, perf_leaf_half);
            int r_possible = last - first_right + 1;
            int free = r_possible - r_count;
            if (free >= how_many) {
                AddToRight(r_ch, first_right, last, first_right, to, how_many, perf_leaf_half);
                return;
            } else {
                AddToRight(r_ch, first_right, last, first_right, to, free, perf_leaf_half);
                how_many -= free;
                AddToRight(taken_.left_child(node_index), first, first_right-1, from, first_right-1, how_many, perf_leaf_half);
            }
        }   
        
        
        // we use including
        int Count(int node_index, int first, int last, int from, int to, int perf_leaf_count) {
            
            if (taken_.node(node_index) == to - from + 1) {
                return last - first + 1;
            }
            if (first == from && last == to) {
                return taken_.node(node_index);
            }
            
            int perf_leaf_half = perf_leaf_count/2;
            int first_right = from + perf_leaf_half;
            
            if (last < first_right) {
                // everything on the left
                return Count(taken_.left_child(node_index), first, last, from, min(first_right - 1, to), perf_leaf_half);
            }
            if (first >= first_right) {
                // everything on the right
                return Count(taken_.right_child(node_index), first, last, first_right, to, perf_leaf_half);
            }
            // everything is between
            return Count(taken_.left_child(node_index), first, first_right - 1, from, first_right - 1, perf_leaf_half) + 
            Count(taken_.right_child(node_index), first_right, last, first_right, to, perf_leaf_half);
        }    
        
        int elem_count_;
        int perf_leaf_count_;
        CompleteBinaryTree<ant::Count> taken_;
        
    };
    
    
    
}

