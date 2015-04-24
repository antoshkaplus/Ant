//
//  grid.cpp
//  ANT
//
//  Created by Anton Logunov on 12/24/14.
//
//

#include "ant/grid.h"

namespace ant {

namespace grid {
    
bool operator==(const Size& s_0, const Size& s_1) {
    return s_0.row == s_1.row && s_0.col == s_1.col;
}

bool operator!=(const Size& s_0, const Size& s_1) {
    return s_0.row != s_1.row || s_0.col != s_1.col;
}

Size operator-(const Size& s_0, const Size& s_1) {
    return {s_0.row - s_1.row, s_0.col - s_1.col};
}

Position operator-(const Position& p, const Indent& n) {
    return {p.row - n.row, p.col - n.col};
}

Position operator+(const Position& p, const Indent& n) {
    return {p.row + n.row, p.col + n.col};
}        

bool operator==(const Position& p_0, const Position& p_1) {
    return p_0.row == p_1.row && p_0.col == p_1.col;
}        

bool operator!=(const Position& p_0, const Position& p_1) {
    return !(p_0 == p_1);
}

Position& operator+=(Position& p, const Size& s) {
    p.row += s.row;
    p.col += s.col;
    return p;
}

bool operator==(const Region& r_0, const Region& r_1) {
    return r_0.position == r_1.position && r_0.size == r_1.size;
}

}

}

