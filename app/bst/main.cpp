
#include <iostream>

#include "bst_set_0.hpp"
#include "bst_set_1.hpp"
#include "bst_set_2.hpp"
#include "bst_set_3.hpp"


using namespace std;
using namespace ant;


// later ... 
vector<vector<int>> GenerateDataSets(int problem_size) {
    

}




template<template<class T, class Compare = std::less<T>> class BstSet>
void Test() {
    default_random_engine rng;
    int N = 1000;
    vector<Index> ns(N);
    iota(ns.begin(), ns.end(), 0);
    shuffle(ns.begin(), ns.end(), rng);
    
    // need to put inside some implementation class probably
    BstSet<Index> bst;
    for (auto n : ns) {
        auto prev_size = bst.size();
        bst.insert(n);
    }
    auto prev = -1;
    for (auto n : bst) {
        if (n <= prev) {
        }
        prev = n;
    }
    
    set<Index> del(ns.begin(), ns.begin()+N/2);
    set<Index> now(ns.begin()+N/2, ns.end());
    for (auto d : del) {
        bst.erase(d);
    }
    for (auto d : del) {
    //    ASSERT_FALSE(bst.exists(d));
    }
    for (auto n : now) {
    //    ASSERT_TRUE(bst.exists(n));
    }
    auto b_it = std::prev(bst.end());
    auto s_it = std::prev(now.end());
    while (true) {
    //    ASSERT_EQ(*b_it, *s_it);
        if (b_it == bst.begin()) break;
        --b_it;
        --s_it;
    }
    
    for (auto n : now) {
        bst.erase(n);
        //ASSERT_FALSE(bst.exists(n));
    }
    //ASSERT_EQ(bst.size(), 0);
    
    bst.insert(ns.begin(), ns.end());
    bst.clear();
    


}

void Test_3() {
    int N = 100;
    BstSet_3<int> bst;
    vector<int> vs(N);
    iota(vs.begin(), vs.end(), 0);
    default_random_engine rng;
    shuffle(vs.begin(), vs.end(), rng);
    bst.insert(vs.begin(), vs.end());
    bst.erase(45);
    auto it = bst.begin();
    for (; it != bst.end(); ++it) {
        cout << *it << " "; 
    }
    cout << endl;
    auto rit = bst.rbegin();
    for (; rit != bst.rend(); ++rit) {
        cout << *rit << " "; 
    }
}





int main(int argc, char **argv) {
//    Test<BstSet_0>();
//    Test<BstSet_1>();
//    Test<BstSet_2>();
    Test_3();
}

