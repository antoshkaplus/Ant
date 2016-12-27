//
//  range_min_interval.hpp
//  Ant
//
//  Created by Anton Logunov on 2/21/16.
//
//

#pragma once



#include "ant/core/core.hpp"

using namespace std;

namespace ant {

// motivation:
//  You are given a sequence A[1], A[2], ..., A[N]. (|A[i]| <= 15007 , 1 <= N <= 50000 ). 
//  A query is defined as follows: 
//  Query(x,y) = Max { a[i]+a[i+1]+...+a[j] ; x <= i <= j <= y }. 
//  Given M queries, your program must output the results of these queries.

// can be generalized: function is called on two elements from left and right of segment tree
// use special data structure, with constructor that takes raw value in

template<class T>
class RangeMinimumInterval {
    
    
    struct Interval {
        T detached_right;
        T detached_left;
        T whole;
        T detached;
    };
    
    struct RightInterval {
        T detached_right;
        T detached;
    };
    
    struct LeftInterval {
        T detached_left;
        T detached;
    };
    
    static LeftInterval left_interval(const Interval& t) {
        return {t.detached_left, t.detached};
    }
    
    static RightInterval right_interval(const Interval& t) {
        return {t.detached_right, t.detached};
    }
    
    static Interval to_interval(const T& t) {
        return Interval{t, t, t, t};
    }
    
    static Interval merge(const Interval& left, const Interval& right) {
        Interval res;
        res.whole = left.whole + right.whole;
        res.detached_left = min(right.detached_left, left.detached_left + right.whole); 
        res.detached_right = min(left.detached_right, right.detached_right + left.whole);
        res.detached = min(right.detached, min(left.detached, right.detached_right + left.detached_left));
        return res;
    }
    
public:
    
    
    // we have to return a little bit different value
    
    // n = 2**(h+1) -1 
    
    // tree consisting only from root node has zero height
    // leaf nodes have zero height
    
    
    RangeMinimumInterval(const std::vector<T>& vs) {
        leaf_count_ = vs.size();
        perfect_leaf_count_ = perfect_leafs(leaf_count_);
        Count nodes_c = perfect_nodes(perfect_leaf_count_) - perfect_leaf_count_ + leaf_count_;
        intervals_.resize(nodes_c);
        std::vector<bool> inited(nodes_c, false);
        for (Index i = 0; i < leaf_count_; ++i) {
            intervals_[i + nodes_c - leaf_count_] = to_interval(vs[i]);
            inited[i + nodes_c - leaf_count_] = true;
        }   
        // parent i/2     
        for (Index i = nodes_c-1; i > 0; --i) {
            // parent
            Index p = (i-1)/2;
            // now we have to merge
            if (!inited[p]) {
                intervals_[p] = intervals_[i];
                inited[p] = true;
            } else { 
                // now we have to merge
                // was initialized with right child
                intervals_[p] = merge(intervals_[i], intervals_[p]); 
            }
        }
    }
    
    // could also return corresponding Index
    T MinimumInterval(Index i, Count n) {
        return MinimumInterval(0, i, n, 0, leaf_count_, perfect_leaf_count_);
    }
    
    void Update(Index i, T val) {
        Update(0, i, perfect_leaf_count_, val);
    }
        
private:
    
    void Update(Index q, Index i, Count n_t, T val) {
        if (n_t == 1) {
            intervals_[q] = to_interval(val);
            return;
        } 
        auto s = n_t / 2;
        if (i < s) {
            Update(left_child(q), i, s, val);
        } else {
            Update(right_child(q), i-s, s, val);
        }
        // interval may not be there at all
        if (right_child(q) < intervals_.size()) {
            intervals_[q] = merge(node_interval(left_child(q)), node_interval(right_child(q)));
        } else {
            intervals_[q] = node_interval(left_child(q));
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
    
    // value is changing. we compute it over time
    T MinimumInterval(Index q, Index i, Count n_i, Index m, Count n_m, Count n_t) {
        if (i == m && n_i == n_m) {
            auto t = node_interval(q);
            return t.detached;
        }
        Count s = n_t / 2;
        // everything on the left
        if (i + n_i <= m + s) {
            return MinimumInterval(left_child(q), i, n_i, m, std::min(s, n_m), s);
        }
        // everything on the right
        if (i >= m + s) {
            return MinimumInterval(right_child(q), i, n_i, m+s, n_m-s, s);
        }
        // first left, last right
        LeftInterval t = MinimumIntervalLeft(left_child(q), i, m, std::min(s, n_m), s);
        RightInterval t_2 = MinimumIntervalRight(right_child(q), i+n_i-1, m+s, n_m-s, s);
        
        // now we have to merge both intervals
        
        return min( t.detached_left+t_2.detached_right, min(t.detached, t_2.detached));
    }
    
    // left subtree
    LeftInterval MinimumIntervalLeft(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m) {
            return left_interval(node_interval(q));
        }
        Count s = n_t / 2;
        // we basically take whole right and something on the left
        if (i < m + s) {
            Interval t = node_interval(right_child(q));
            LeftInterval t_left = MinimumIntervalLeft(left_child(q), i, m, std::min(n_m, s), s);
            LeftInterval res;
            res.detached = min(t.detached_right + t_left.detached_left, min(t.detached, t_left.detached));
            res.detached_left = min(t.detached_left, t.whole + t_left.detached_left);
            return res;
        } 
        return MinimumIntervalLeft(right_child(q), i, m+s, n_m-s, s);
    }
    
    // right subtree
    RightInterval MinimumIntervalRight(Index q, Index i, Index m, Count n_m, Count n_t) {
        if (i == m + n_m - 1) {
            return right_interval(node_interval(q));
        }
        Count s = n_t / 2;
        if (i < m + s) {
            return MinimumIntervalRight(left_child(q), i, m, std::min(n_m, s), s);
        }
        Interval t = node_interval(left_child(q));
        RightInterval t_right = MinimumIntervalRight(right_child(q), i, m+s, n_m - s, s);
        RightInterval res;
        res.detached = min(t.detached_left + t_right.detached_right, min(t.detached, t_right.detached));
        res.detached_right = min(t.detached_right, t.whole + t_right.detached_right);
        return res;
    }
    
    
    
    // children: 2*i, 2*i+1 +1 because started with 0
    Index left_child(Index parent) {
        return 2*parent + 1;
    }
    
    Index right_child(Index parent) {
        return 2*parent + 2;
    }
    
    const Interval& node_interval(Index node) {
        return intervals_[node];
    } 
    
    Count perfect_leaf_count_;
    Count leaf_count_;
    std::vector<Interval> intervals_;
}; 

}

