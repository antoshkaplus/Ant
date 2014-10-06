//
//  bigint.cpp
//  Ant
//
//  Created by Anton Logunov on 7/16/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <stdio.h>

#include "ant/bigint.h"


namespace ant {

bigint standard_multiplication(const bigint& b_0, const bigint& b_1) {
    bigint r;
    if (b_0.is_zero() || b_1.is_zero()) {
        r.words_.resize(1, 0);
        return r;
    }
    
    auto& w_0 = b_0.words_;
    auto& w_1 = b_1.words_;
    r.is_negative_ = b_0.is_negative_ ^ b_1.is_negative_; // should be bit operation 
                                                          // b_1 * b_0
                                                          // inner loop should be bigger?
    auto& w_r = r.words_; 
    w_r.resize(w_0.size() + w_1.size(), 0);
    for (auto i = 0; i < w_0.size(); ++i) {
        for (auto j = 0; j < w_1.size(); ++j) {
            w_r[i+j] += w_0[i]*w_1[j];
            w_r[i+j+1] += w_r[i+j]/bigint::kWordBase;
            w_r[i+j] %= bigint::kWordBase; 
        }
    }
    if (w_r.back() == 0) w_r.pop_back();
    return r;
}

//bigint karatsuba_multiplication(const bigint& b_0, const bigint& b_1) {
//    bigint r;
//}


bigint operator*(const bigint& b_0, const bigint& b_1) {
    return standard_multiplication(b_0, b_1);
}

std::ostream& operator<<(std::ostream& output, const bigint& b) {
    auto& w = b.words_;
    if (b.is_negative_ && w.back() != 0) output << '-';
    output << w.back();
    
    for (Int i = (Int)w.size()-2; i >= 0; --i) {
        output.width(bigint::kWordDigitCount);
        output.fill('0');
        output.setf(std::ios::right);
        
        output << w[i];
    }
    output.width(0);
    return output;
}   


}