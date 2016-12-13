//
//  skip_list.cpp
//  Ant
//
//  Created by Anton Logunov on 12/4/16.
//
//

#include "gtest/gtest.h"

#include "ant/core/skip_list_counter.hpp"
#include "ant/core/skip_list_set.hpp"
#include "ant/core/skip_list_op.hpp"

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

TEST(SkipListSet, allin) {
    int opCount = 10000;
    
    // we do a bunch of operations on set, vector and skip_list
    vector<int> vs; 
    set<int> ss;
    SkipListSet<int> sls(opCount);
    
    int min_value = 1;
    int max_value = 10000;

    uniform_int_distribution<> value_distr(min_value, max_value);

    default_random_engine rng;
    discrete_distribution<int> op_distr({0.5, 0.2});

    int op;
    for (auto i = 0; i < opCount; ++i) {
        // 0 - push_back 0.5
        // 1 - get|remove at position and swap 0.2
        // at the end get|remove everything try to access after removal
        op = op_distr(rng);
        if (vs.empty()) {
            op = 0;
        }
        
        switch (op) {
            case 0: {
                auto val = value_distr(rng);
                if (ss.count(val) != 1) {
                    vs.push_back(val);
                    ss.insert(val);
                    sls.insert(val);
                } 
                break;
            }
            case 1: { 
                uniform_int_distribution<> pos_distr(0, vs.size()-1);
                auto pos = pos_distr(rng);
                auto val = vs[pos];
                SwapBackPop(vs, pos);
                ss.erase(val);
                ASSERT_EQ(sls[val], val);
                sls.remove(val);
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }
    }
    for (auto v : vs) {
        sls.remove(v);
        ASSERT_EQ(sls.at(v), nullptr);
    }
}


TEST(SkipListOp, allin) {
    auto op = [](int i_1, int i_2) {
        return max(i_1, i_2);
    };

    OpSkipList<int, decltype(op)>(100, op);


}



