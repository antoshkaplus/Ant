//
//  bigint.cpp
//  Ant
//
//  Created by Anton Logunov on 8/24/16.
//
//


#include "gtest/gtest.h"

#include "ant/bigint.hpp"


namespace {
    
    
using namespace ant;
using namespace std;

TEST(bigint, sum) {
    bigint b_1("9999999999999999999");
    bigint b_2("1");
    bigint b_r("10000000000000000000");
    auto b_rr = sum(b_1, b_2);
    ASSERT_EQ(b_r, b_rr);
}    
    
TEST(bigint, remainder) {
    
}
 
    
TEST(bigint, CountDigits) {
    int d_max = 100;
    for (int i = 0; i < d_max; ++i) {
        char ch = static_cast<char>('0'+ i%8 + 1);
        auto count = CountDigits(bigint(string(i, ch)));
        ASSERT_EQ(i, count);
    }
    auto c = CountDigits(bigint("0"));
    ASSERT_EQ(c, 0);
}
    
    
    
}