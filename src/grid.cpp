//
//  grid.cpp
//  ANT
//
//  Created by Anton Logunov on 12/24/14.
//
//

#include "ant/grid.hpp"

namespace ant {

namespace grid {
    
    
bool operator!=(const Indent& d_0, const Indent& d_1) {
    return d_0.row != d_1.row || d_0.col != d_1.col;
}   

bool operator==(const Indent& d_0, const Indent& d_1) {
    return d_0.row == d_1.row && d_0.col == d_1.col;
}

    
Direction FromDirVector(Indent ind) {
    for (auto d = 0; d < kDirCount; ++d) {
        if (kDirVector[d] == ind) {
            return d;
        }
    }
    throw logic_error("FromDirVector from input");
}


bool operator==(const Size& s_0, const Size& s_1) {
    return s_0.row == s_1.row && s_0.col == s_1.col;
}

bool operator!=(const Size& s_0, const Size& s_1) {
    return s_0.row != s_1.row || s_0.col != s_1.col;
}

Size operator-(const Size& s_0, const Size& s_1) {
    return {s_0.row - s_1.row, s_0.col - s_1.col};
}



Position Position::Shifted(Direction dir) {
    return *this + kDirVector[dir];
}

Position operator-(const Position& p, const Indent& n) {
    return {p.row - n.row, p.col - n.col};
}

Position operator+(const Position& p, const Indent& n) {
    return {p.row + n.row, p.col + n.col};
}        

Position operator-(const Indent& n, const Position& p) {
    return {p.row - n.row, p.col - n.col};
}

Position operator+(const Indent& n, const Position& p) {
    return {p.row + n.row, p.col + n.col};
}        

bool operator==(const Position& p_0, const Position& p_1) {
    return p_0.row == p_1.row && p_0.col == p_1.col;
}        

bool operator!=(const Position& p_0, const Position& p_1) {
    return !(p_0 == p_1);
}

    
Position Centroid(const Position& p_0, const Position& p_1) {
    return { (p_0.row + p_1.row)/2, (p_0.col + p_1.col)/2 }; 
}

Position& operator+=(Position& p, const Size& s) {
    p.row += s.row;
    p.col += s.col;
    return p;
}

bool operator==(const Region& r_0, const Region& r_1) {
    return r_0.position == r_1.position && r_0.size == r_1.size;
}

Indent operator-(const Position& p_0, const Position& p_1) {
    return {p_0.row - p_1.row, p_0.col - p_1.col}; 
}

Grid<char> ToGrid(const std::vector<std::string>& ss) {
    Grid<char> g(ss.size(), ss[0].size());
    auto it = g.begin();
    for (auto& s : ss) {
        for (auto& c : s) {
            *it++ = c;
        }
    }
    return g;
} 



}

}

