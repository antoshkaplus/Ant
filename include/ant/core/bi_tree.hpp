//
//  bi_tree.hpp
//  Ant
//
//  Created by Anton Logunov on 12/22/16.
//  
//  origins:
//  http://www.geeksforgeeks.org/binary-indexed-tree-or-fenwick-tree-2/
//  https://www.youtube.com/watch?v=v_wj_mOAlig
//  range update:
//  http://petr-mitrichev.blogspot.com/2013/05/fenwick-tree-range-updates.html
//  https://github.com/guidotag/Fenwick-Tree/blob/master/RangeFenwickTree.hpp

#pragma once

namespace ant {

// only for reversable operations
// don't see any other application other than with sum
template<class Val>
class BITree {

public:
    
    BITree(const vector<Val>& vs) {
    
        tree.resize(vs.size()+1, 0);
        
        for (Index i = 0; i < vs.size(); ++i) {
            update(i, vs[i]);
        }
    }
    
    Val rangeSum(Index i_1, Index i_2) {
        return sumTo(i_2) - sumTo(i_1-1);
    }
    
    // Returns sum of arr[0..index]
    Val sumTo(Index idx) {
        Val sum = 0; // Iniialize result
        for (auto i = idx+1; i > 0; i -= lastBit(i)) {
            sum += tree[index];
        }
        return sum;
    }
    
    void update(Index idx, int val) {
        for (auto i = idx+1; i < tree.size(); i += lastBit(i)) {
            tree[index] += val;
        }
    }
    
    
private:

    Index lastBit(Index idx) {
        return idx & (-idx); 
    }

    vector<Val> tree;

};


}
