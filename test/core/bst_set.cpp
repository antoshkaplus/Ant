
#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/core/bst_set.hpp"
#include "ant/core/avl_set.hpp"

namespace {
    
    
using namespace ant;
using namespace std;


TEST(BstSet, allin) {
    default_random_engine rng;
    Count N = 1000;
    vector<Index> ns(N);
    iota(ns.begin(), ns.end(), 0);
    shuffle(ns.begin(), ns.end(), rng);
    
    BstSet<Index> bst;
    ASSERT_EQ(bst.size(), 0);
    for (auto n : ns) {
        auto prev_size = bst.size();
        bst.insert(n);
        ASSERT_EQ(bst.size(), prev_size+1);
    }
    auto prev = -1;
    for (auto n : bst) {
        if (n <= prev) {
            FAIL();
        }
        prev = n;
    }
    ASSERT_EQ(*bst.begin(), 0);
    ASSERT_EQ(*std::prev(bst.end()), N-1);
    
    set<Index> del(ns.begin(), ns.begin()+N/2);
    set<Index> now(ns.begin()+N/2, ns.end());
    for (auto d : del) {
        bst.erase(d);
    }
    for (auto d : del) {
        ASSERT_FALSE(bst.exists(d));
    }
    for (auto n : now) {
        ASSERT_TRUE(bst.exists(n));
    }
    auto b_it = std::prev(bst.end());
    auto s_it = std::prev(now.end());
    while (true) {
        ASSERT_EQ(*b_it, *s_it);
        if (b_it == bst.begin()) break;
        --b_it;
        --s_it;
    }
    
    for (auto n : now) {
        bst.erase(n);
        ASSERT_FALSE(bst.exists(n));
    }
    ASSERT_EQ(bst.size(), 0);
    
    bst.insert(ns.begin(), ns.end());
    bst.clear();
    
}






}