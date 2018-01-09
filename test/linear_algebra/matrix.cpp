
#include <fstream>
#include <array>
#include <functional>

#include "gtest/gtest.h"

#include "ant/linear_algebra/matrix.h"
#include "ant/geometry/d2.hpp"

namespace {
    
    using namespace ant::linalg;
    
    TEST(matrix, multiplication) {
        Matrix<int> m(2, 2);
        m(0, 0) = 1;
        m(0, 1) = 0;
        m(1, 0) = 0;
        m(1, 1) = 1;
        auto r = (m == m*m);
        ASSERT_TRUE(r(0, 0) == true && r(0, 1) == true
                    && r(1, 0) == true && r(1, 1) == true);
    }
    
    TEST(matrix, minus) {
    
    }
    
    TEST(matrix, plus) {
    
    }
    
    TEST(matrix, logical_or) {
    
    }
    // unary operator
    // logical
    // relational vs Comparison Operators
    // arithmetical
    // bitwise ?? 
    // overloaded typecast operator // operator int() const { return side * side; }
    
    TEST(matrix, all) {
        Matrix<bool> m(2, 2);
        for (int i = 0; i < 4; ++i) {
            m[i] = true;
        }
        ant::linalg::all(m);
    }
    
}