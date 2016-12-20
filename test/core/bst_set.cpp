
#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/core/bst.hpp"
#include "ant/core/avl_tree.hpp"
#include "ant/core/rb_tree_2.hpp"

namespace {
    
    
using namespace ant;
using namespace std;


TEST(BstSet, allin) {
    default_random_engine rng;
    Count N = 1000;
    vector<Index> ns(N);
    iota(ns.begin(), ns.end(), 0);
    shuffle(ns.begin(), ns.end(), rng);
    
    Bst<Index> bst;
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

TEST(RB_Tree, allin) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    int i = 0;
    for (auto v : vs) {
        rb_tree.put(v);
        ASSERT_EQ(rb_tree.size(), ++i);
    }
    i = 0;
    auto func = [&](int k) {
        ASSERT_EQ(i, k);
        ++i;
    };
    rb_tree.forEach(func);
    ASSERT_EQ(i, N);
}

TEST(RB_Tree, PutAt) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    for (auto it = vs.begin(); it != vs.end(); ++it) {
        auto const insertion = std::upper_bound(vs.begin(), it, *it);
        rb_tree.putAt(insertion-vs.begin(), *it);
        std::rotate(insertion, it, std::next(it));
        
    }    
    auto i = 0;
    auto func = [&](int k) {
        ASSERT_EQ(i, k);
        ++i;
    };
    rb_tree.forEach(func);
    ASSERT_EQ(i, N);
}

TEST(RB_Tree, Query) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    for (auto i = 0; i < vs.size(); ++i) {
        rb_tree.putAt(i, vs[i]);
    }
    auto i = 0;
    auto func = [&](int k) {
        ASSERT_EQ(vs[i], k);
        ++i;
    };
    rb_tree.forEach(func);
    
    ASSERT_EQ(rb_tree.query(0, vs.size()-1), accumulate(vs.begin(), vs.end(), 0));
    
    uniform_int_distribution<> distr(0, vs.size()-1);
    for (auto i = 0; i < N; ++i) {
        auto k_1 = distr(rng);
        auto k_2 = distr(rng);
        tie(k_1, k_2) = minmax(k_1, k_2);
        auto vs_sum = accumulate(vs.begin()+k_1, vs.begin()+k_2+1, 0);
        auto rb_sum = rb_tree.query(k_1, k_2);
        ASSERT_EQ(vs_sum, rb_sum);
    }
}

TEST(RB_Tree, GetIndex) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    for (auto i = 0; i < vs.size(); ++i) {
        rb_tree.put(vs[i]);
    }
    
    sort(vs.begin(), vs.end());
    
    for (int i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(rb_tree.getIndex(vs[i]), i);
    }
}

TEST(RB_Tree, GetAt) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    for (auto i = 0; i < vs.size(); ++i) {
        rb_tree.put(vs[i]);
    }
    
    sort(vs.begin(), vs.end());
    
    for (int i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(rb_tree.getAt(i), vs[i]);
    }
}

TEST(RB_Tree, UpdateAt) {
    int N = 1000;
    vector<int> vs(N);
    default_random_engine rng;
    iota(vs.begin(), vs.end(), 0);
    shuffle(vs.begin(), vs.end(), rng);
    
    auto sum = [](int s_1, int s_2) {
        return s_1 + s_2;
    };
    RB_Tree<int, decltype(sum)> rb_tree(sum);
    for (auto i = 0; i < vs.size(); ++i) {
        rb_tree.putAt(i, vs[i]);
    }
        
    shuffle(vs.begin(), vs.end(), rng);
    for (auto i = 0; i < vs.size(); ++i) {
        rb_tree.updateAt(i, vs[i]);
    }
    
    uniform_int_distribution<> distr(0, vs.size()-1);
    for (auto i = 0; i < N; ++i) {
        auto k_1 = distr(rng);
        auto k_2 = distr(rng);
        tie(k_1, k_2) = minmax(k_1, k_2);
        auto vs_sum = accumulate(vs.begin()+k_1, vs.begin()+k_2+1, 0);
        auto rb_sum = rb_tree.query(k_1, k_2);
        ASSERT_EQ(vs_sum, rb_sum);
    }
}





}
