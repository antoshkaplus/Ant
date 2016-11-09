//
//  str.hpp
//  Ant
//
//  Created by Anton Logunov on 10/22/16.
//
//

#pragma once

#include "ant/core/core.hpp"
#include "ant/grid/grid.hpp"

namespace ant {


    
class LCS { 
    using V_b = std::vector<bool>;
    using S = const std::string;
    
    Grid<int> dp;
    S *s_1_;
    S *s_2_;
    
    V_b *b_1_;
    V_b *b_2_;
    
    int sz_1;
    int sz_2;
    
public:
    LCS() {}
    
    std::tuple<V_b, V_b> compute(const std::string& s_1, const std::string& s_2){
        s_1_ = &s_1;
        s_2_ = &s_2;
        
        sz_1 = s_1.size();
        sz_2 = s_2.size();
        
        V_b b_1(sz_1, false);
        V_b b_2(sz_2, false);
        
        b_1_ = &b_1;
        b_2_ = &b_2; 
        
        dp.resize(sz_1+1, sz_2+1);
        
        CalcDP();
        Backtrack();
        
        return std::tuple<V_b, V_b>{std::move(b_1), std::move(b_2)};
    }   
    
private:
    
    void CalcDP() {
        auto& s_1 = *s_1_;
        auto& s_2 = *s_2_;
        
        for(int i = 0; i <= sz_1; i++) {
            dp(i, 0) = 0;
        }
        for(int j = 0; j <= sz_2; j++) {
            dp(0, j) = 0;
        }
        
        //  Calculate the matrix for LCS.
        for(int i = 1; i <= sz_1; i++){
            for(int j = 1; j <= sz_2; j++){
                if(s_1[i-1] == s_2[j-1]) {
                    dp(i, j) = dp(i-1, j-1) + 1;
                } else {
                    dp(i, j) = std::max(dp(i-1, j), dp(i, j-1));
                }
            }
        }
    }
    
    void Backtrack() {
        auto& s_1 = *s_1_;
        auto& s_2 = *s_2_;
        auto& b_1 = *b_1_; 
        auto& b_2 = *b_2_;
        
        int x = sz_1;
        int y = sz_2;
        while(true){
            if(x == 0 || y == 0)
                break;
            //  If two characters are equal, 
            //      move to diagonally opposite cell and mark the current position in b_1 and b_2 as true.
            //  else, move to the cell [x-1][y] or [x][y-1], whichever has the maximum value.
            if(s_1[x-1] == s_2[y-1]){
                b_1[x-1] = true;
                b_2[y-1] = true;
                --x;
                --y;
            } else if (dp(x-1, y) >= dp(x, y-1)) {
                --x;
            } else {
                --y;
            }
        }
    }
};



}
