//
//  skip_list.cpp
//  Ant
//
//  Created by Anton Logunov on 12/4/16.
//
//

#include "gtest/gtest.h"

#include "ant/core/skip_list.hpp"

using namespace ant;
using namespace std;


// iterator implementation is needed pretty badly
// maybe
void Print(const CounterSkipList<int>& cs) {
    auto cur = cs.head->next[0];
    while (cur != cs.tail) {
        // value, all counters for each height
        cout << cur->value << " | ";
        for (auto i = 0; i < cur->numAfterPrev.size(); ++i) {
            cout << cur->numAfterPrev[i] << ", ";
        }
        cout << endl;
        cur = cur->next[0];
    }
    cout << "last: ";
    for (auto i = 0; i < cur->numAfterPrev.size(); ++i) {
        cout << cur->numAfterPrev[i] << ", ";
    }
    cout << endl;
}



TEST(CounterSkipList, allin) {
    int opCount = 100000;
    
    // operation, get, remove, insert
    vector<int> vs;
    CounterSkipList<int> cs(opCount);
    discrete_distribution<int> opDistr({0.4, 0.1, 0.5});
    default_random_engine rng;
    for (auto i = 0; i < opCount; ++i) {
        auto op = opDistr(rng);
        if (vs.size() == 0) {
            op = 0;
        }
        switch (op) {
            case 0: { // insert
                uniform_int_distribution<> idxDistr(0, vs.size());
                Index idx = idxDistr(rng);
                
                vs.insert(vs.begin() + idx, i);
                cs.insertAt(idx, i);
                break;
            }
            case 1: { // remove
                uniform_int_distribution<> idxDistr(0, vs.size()-1);
                Index idx = idxDistr(rng);
                
                vs.erase(vs.begin() + idx);
                cs.removeAt(idx);
                break;
            }
            case 2: { // get
                uniform_int_distribution<> idxDistr(0, vs.size()-1);
                Index idx = idxDistr(rng);
                
                ASSERT_EQ(vs[idx], cs[idx]);
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }
    }
    for (auto i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(vs[i], cs[i]);
    }
}