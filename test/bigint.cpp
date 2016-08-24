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
    
    
    
    
}