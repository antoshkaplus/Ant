//
//  main.cpp
//  TestLinAlg
//
//  Created by Anton Logunov on 7/1/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <iostream>
#include <valarray>

#include "matrix.h"

using namespace ant::linalg;
using namespace std;

int main(int argc, const char * argv[]) {
    int 
    r_count = 1000,
    c_count = 1000;
    Size size(r_count, c_count);
    Matrix<int> m_0(size), m_1(size), m_2(size);
    m_0.fill(2);
    m_1.fill(3);
    m_2.fill(4);
    
    int k = 1000;
    
    
    valarray<int> v_0(size.row*size.col), v_1(v_0.size()), v_2(v_0.size());
    clock_t t = clock();
    for (auto i = 0; i < k; ++i) {
        v_0 += v_0 * v_1 + 89 - v_2;
    }
    cout << clock() - t << "\n";
    
    t = clock();
    for (auto i = 0; i < k; ++i) {
        m_0 += m_0 % m_1 + 89 - m_2; 
    }
    cout << clock() - t << "\n";
    
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
