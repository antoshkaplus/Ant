//
//  knuth_morris_pratt.cpp
//  Ant
//
//  Created by Anton Logunov on 8/23/15.
//
//

#include "gtest/gtest.h"

#include "ant/core/knuth_morris_pratt.hpp"



using namespace ant;
using namespace std;

TEST(KMP, all_in) {
    
    
    string pat = "abs";
    KnuthMorrisPratt<string::const_iterator> kmp(pat.begin(), pat.end());
    
    const string s_0 = "fbroqbgoqogbq";
    const string s_1 = "abs";
    const string s_2 = "abasbsabsbbaa";
    
    auto r_0 = kmp(s_0.begin(), s_0.end());
    ASSERT_EQ(r_0, s_0.end());
    auto r_1 = kmp(s_1.begin(), s_1.end());
    ASSERT_NE(r_1, s_1.end());
    auto r_2 = kmp(s_2.begin(), s_2.end());
    ASSERT_NE(r_2, s_2.end());     
}