//
//  bigint.h
//  Ant
//
//  Created by Anton Logunov on 7/16/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __ANT_BIGINT_H__
#define __ANT_BIGINT_H__

#include <string>

#include "ant/core/core.hpp"

namespace ant {

//struct
//bigint operator*(const bigint& b_0, const bigint& b_1);
    
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
    
    
private:
    using word_type = int64_t;
    
    constexpr static const auto kStrToWord = std::atol;
    static const size_t kWordDigitCount = 9;
    static const word_type kWordBase = 1e+9;
    
    bool is_negative_;
    std::vector<word_type> words_;
    // how many word should be empty on the right
    Count shift_;
    
    
    friend bigint operator*(const bigint& b_0, const bigint& b_1);
    friend bigint standard_multiplication(const bigint& b_0, const bigint& b_1);
    friend bigint sum(const bigint& b_1, const bigint& b_2);
    friend std::ostream& operator<<(std::ostream& output, const bigint& b);
};

// one interface but different implementations???
struct bigint_view {
    
    Count shift_;
    // should be a range kind of
    Index i_begin;
    Index i_end;
    bigint& base;
};


}

#endif