//
//  bigint.cpp
//  Ant
//
//  Created by Anton Logunov on 7/16/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <stdio.h>

#include "ant/bigint.hpp"


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



bigint sum(const bigint& b_1, const bigint& b_2) {
    
	auto w_min_p = &b_1.words_;
	auto w_max_p = &b_2.words_;
	if (w_min_p->size() > w_max_p->size()) {
		swap(w_min_p, w_max_p);
	}
	auto& w_min = *w_min_p;
	auto& w_max = *w_max_p; 
	
	auto min_sz = w_min.size();
	auto max_sz = w_max.size();
	
	bigint b_r;
    b_r.is_negative_ = false;
    auto& w_r = b_r.words_;
	w_r = w_max;
	for (auto i = 0; i < min_sz; ++i) {
		w_r[i] += w_min[i];	
    }
	w_r.push_back(0);
    for (auto i = 0; i < max_sz; ++i) {
		b_r.ShiftExcessiveRanks(i);
	}
	if (w_r.back() == 0) w_r.pop_back();
	
	return b_r;
}

bigint division(const bigint& b, int small_numb) {
	bigint bb = b;
	auto& w = bb.words_;
	for (auto i = w.size()-1; i > 0; --i) {
		auto t = w[i] % small_numb;
		w[i] /= small_numb;
		w[i-1] += bigint::kWordBase*t;
		
	}
	w[0] /= small_numb;
	return bb;
}

int remainder(const bigint& b, int small_numb) {
	return b.words_.back() % small_numb;
}

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

bool operator==(const bigint& b_1, const bigint& b_2) {
    return b_1.is_negative_ == b_2.is_negative_ && b_1.words_ == b_2.words_;
}

bool operator!=(const bigint& b, int small) {
    return (b.words_.empty() && small == 0) || (b.words_.size() == 1 && b.words_.back() == small);
}

}
