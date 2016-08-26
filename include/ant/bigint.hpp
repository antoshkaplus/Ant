//
//  bigint.h
//  Ant
//
//  Created by Anton Logunov on 7/16/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//
//  When integer equals 0, words vector is empty.
//  This will help us to preserve consistency.


#ifndef __ANT_BIGINT_H__
#define __ANT_BIGINT_H__

#include <string>

#include "ant/core/core.hpp"

namespace ant {

    
struct bigint {
    
    bigint() {}
    bigint(const std::string& s) {
        auto s_i_begin = 0;
        while (std::isspace(s[s_i_begin])) {
            ++s_i_begin;
        }
        
        is_negative_ = false;
        if (s[s_i_begin] == '-') is_negative_ = true;
        if (s[s_i_begin] == '-' || s[s_i_begin] == '+') ++s_i_begin;
        
        Count n = 0;
        while (std::isdigit(s[s_i_begin + n])) {
            ++n;
        }
        init_words(s.c_str()+s_i_begin, n);
    }  
    
    bigint(int small) {
        is_negative_ = false;
        words_.push_back(small);
    }
    
    void init_words(const char* s, Count n) {
        if (n == 0) {
            words_.resize(1);
            words_[0] = 0;
        }
        
        auto w_count = n/kWordDigitCount;
        auto rem = n%kWordDigitCount;
        char buf[kWordDigitCount+1]; // 0 character 
        auto assign = [&](const char* str, Count str_n, Index w_i) {
            std::memcpy(buf, str, str_n*sizeof(char));
            buf[str_n] = '\0';
            words_[w_i] = kStrToWord(buf);
        };
        if (rem > 0) { ;
            words_.resize(w_count+1);
            assign(s, rem, w_count);
        } else {
            words_.resize(w_count);
        }
        for (Int w_i = 0, s_i = (Int)n-kWordDigitCount; 
             w_i < w_count; ++w_i, s_i-=kWordDigitCount) {
            assign(s+s_i, kWordDigitCount, w_i);
        }
    }
    
    bool is_zero() const {
        return words_.size() == 1 && words_[0] == 0;
    }
    
    Count digit_count() const {
		if (words_.empty()) return 0;
		return CountDigits(words_.back()) + (kWordDigitCount * (words_.size() - 1));
	}
    
    void Sum(const bigint& b) {
        
    }
    
    int Remainder(int small) {
        return remainder(*this, small);
    }
    
    void Divide(int small) {
        division(*this, small);
    }
    
    
    void Mul(int small_numb) {
        if (words_.empty()) return;
        auto sz = words_.size();
        for (auto i = 0; i < sz; ++i) {
            words_[i] *= small_numb;
        }
        words_.push_back(0);
        for (auto i = 0; i < sz; ++i) {
            ShiftExcessiveRanks(i);
        }
        if (words_.back() == 0) words_.pop_back();
    }

    
	
private:

	void ShiftExcessiveRanks(Index i) {
		auto& w = words_;
		assert(w.size() > i+1);
		w[i+1] += w[i]/kWordBase;
        w[i] %= kWordBase; 
	}
	
    using word_type = int64_t;
    
    constexpr static const auto kStrToWord = std::atol;
    static const size_t kWordDigitCount = 9;
    static const word_type kWordBase = 1e+9;
    
    // should ve just use negative
    bool is_negative_;
    std::vector<word_type> words_;
    // how many word should be empty on the right
    Count shift_;
    
    
    friend bigint operator*(const bigint& b_0, const bigint& b_1);
    friend bigint standard_multiplication(const bigint& b_0, const bigint& b_1);
    friend bigint sum(const bigint& b_1, const bigint& b_2);
    friend std::ostream& operator<<(std::ostream& output, const bigint& b);
    friend bigint division(const bigint& b, int small_numb);
    friend int remainder(const bigint& b, int small_numb);
    
    
    friend bool operator==(const bigint& b_1, const bigint& b_2);
    friend bool operator!=(const bigint& b, int small);
};

// one interface but different implementations???
struct bigint_view {
    
    Count shift_;
    // should be a range kind of
    Index i_begin;
    Index i_end;
    bigint& base;
};


std::string ToString(const bigint& n) {
    if (n.words_.empty()) {
		return "0";
	}
	std::string s;
	s.reserve(word_.size()*bigint::kWordDigitCount);
	while (n != 0) {
		int d = n.Remainder()
		s.push_back(d + '0');
		n.Divide(10);
	}
	std::reverse(s.begin(), s.end());
	return s;
}




}

#endif
