//
//  optimization.cpp
//  ANT
//
//  Created by Anton Logunov on 1/3/15.
//
//

#include <stdio.h>
#include <cmath>
#include <algorithm>

#include "gtest/gtest.h"

#include "ant/optimization.h"


namespace {

using namespace std;
using namespace ant::opt;

TEST(GoldenSectionSearch, test) {
    using S = pair<double, double>;
    using F = function<double(double)>;
    using P = pair<F, S>;
    vector<P> ps = {
        { [](double x) { return 100.*pow(x-0.24, 2); },
          {0, 1} },
        
        { [](double x) { return pow(x, 2) + 16./x; }, 
          {1, 4} },
          
        { [](double x) { return pow(x - 1, 4); },
          {-2, 3} },
    
        { [](double x) { return x * atan(x) - log(1 + x*x)/2.; },
          {-6, 6} },
          
        { [](double x) { return x * sin(1/x); },
          {0.2, 1} } 
    };
    double eps = 1e-4;
    for (auto p : ps) {
        auto f = p.first;
        auto s = p.second;
        double a = s.first;
        double b = s.second;
        double x = GoldenSectionSearch(a, b, f, eps);
        double f_x = f(x);
        ASSERT_TRUE(f_x < f(x-eps)) << f_x << " " << f(x-2*eps); 
        ASSERT_TRUE(f_x < f(x+eps)) << f_x << " " << f(x+2*eps);
        cout << "a: " << s.first << " b: " << s.second << " x_min: " << x << endl;
    }
}





TEST(Node, test_Erase) {
    int count = 10;
    auto prepare = [=]() {
        shared_ptr<Node<int>> node, prev_node;
        for (int i = 0; i < count; ++i) {
            prev_node = node;
            node.reset(new Node<int>());
            node->value = i;
            node->previous = prev_node;
        }
        return node;
    };
    
    // erase first, middle, last
    for (int i : {0, 3, 9}) {
        auto s = prepare();
        s = Node<int>::Erase(s, i);
        auto v = Node<int>::ToVector(s);
        ASSERT_TRUE(v.size() == count-1);
        ASSERT_TRUE(std::count(v.begin(), v.end(), i) == 0);
    }
}


TEST(Node, test_DeepCopy) {
    int count = 10;
    auto prepare = [=]() {
        shared_ptr<Node<int>> node, prev_node;
        for (int i = 0; i < count; ++i) {
            prev_node = node;
            node.reset(new Node<int>());
            node->value = i;
            node->previous = prev_node;
        }
        return node;
    };
    
    for (int i : {0, 3, 9}) {
        auto s = prepare();
        auto copy = Node<int>::DeepCopy(s);
        s = Node<int>::Erase(s, i);
        auto v = Node<int>::ToVector(copy);
        ASSERT_TRUE(v.size() == count);
        for (int i = 0; i < count; ++i) {
            ASSERT_TRUE(v[i] == i);
        }
    }
}

TEST(Node, test_Exists) {
    int count = 10;
    auto prepare = [=]() {
        shared_ptr<Node<int>> node, prev_node;
        for (int i = 0; i < count; ++i) {
            prev_node = node;
            node.reset(new Node<int>());
            node->value = i;
            node->previous = prev_node;
        }
        return node;
    };
    
    vector<int> i = {   0,    23,    3,    -1,    9};
    vector<int> b = {true, false, true, false, true};
    auto s = prepare();
    for (int k = 0; k < i.size(); ++k) {
        ASSERT_TRUE(Node<int>::Exists(s, i[k]) == b[k]);
    }
}

             
}