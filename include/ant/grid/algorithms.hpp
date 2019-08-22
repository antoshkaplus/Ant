//
//  algorithms.hpp
//  Ant
//
//  Created by Anton Logunov on 11/15/15.
//
//

#pragma once

#include "ant/grid/grid.hpp"

namespace ant {
namespace grid {



// need special structure to keep 

// IsNeighbor : takes position and direction is, returns bool

// what should be result... possible paths to the target
template<class T, class IsNeighbor>
// put everything in...
// we need paths ....
std::vector<std::vector<Position>> BFSS(Grid<T>& g, Position& origin, Position& target, IsNeighbor& is_neighbor) {
    // did come here from direction
//    Grid<std::array<bool, 4>> s(g.size());
//    s.fill(-1);
//    s[origin] = 0;
//    std::queue<Position> st;
//    st.push(origin);
//    
//    short min_distance = 0;
//    
//    while (!st.empty()) {
//        auto t = st.top();
//        st.pop();
//        
//    }
    return {};
}

template<class T, class IsNeighbor>
class BFS {
public:
    void Init(const Grid<T>& g, const IsNeighbor& is_neighbor) {
        g_ = &g;
        is_neighbor_ = &is_neighbor;
        from_.resize(g.size());
        distance_.resize(g.size());
    }
    
    std::vector<std::vector<Position>> FindShortestPaths(const Position& origin, const Position& target) {
        from_.fill({{false, false, false, false}});
        distance_.fill(std::numeric_limits<short>::max());
        distance_[origin] = 0;
        std::queue<Position> st;
        st.push(origin);
        while (!st.empty()) {
            auto t = st.front();
            st.pop();
            if (t == target) {
                continue;
            }
            auto ddd = distance_(t);
            
            for (auto d : kDirections) {
                Position p;
                if (is_neighbor(t, d) && ddd+1 <= distance_( p = t.Shifted(d))) {
                    assert(p.row >= 0 && p.col >= 0);
                    if (ddd + 1 == distance_(p)) {
                        from_[p][kDirOpposite[d]] = true;
                    }
                    // in one wave
                    else if (ddd + 1 < distance_(p)) {
                        from_[p] = {{false, false, false, false}};
                        from_[p][kDirOpposite[d]] = true;
                        distance_[p] = ddd + 1;
                        st.push(p);
                    }
                }
            }
        }
        return GatherPaths(origin, target); 
    }
    
private: 
    
    
    // here better just stick with recursion
    
    // for recursion need to pass current vector + all other...
    // or we could just use our data structure with lists
    
    // lets go with iterative solution
    std::vector<std::vector<Position>> GatherPaths(const Position origin, const Position target) {
        std::vector<std::vector<Position>> res;
        // next position to expand, vector index
        std::stack<std::tuple<Position, Index>> st;
        res.push_back(std::vector<Position>{target});
        st.push({target, 0});
        Position p;
        Index i;
        while (!st.empty()) {
            std::tie(p, i) = st.top();
            st.pop();
            if (p == origin) continue;
            bool used = false;
            for (auto k : kDirections) {
                if (from_[p][k]) {
                    auto pp = p.Shifted(k);
                    if (!used) {
                        res[i].push_back(pp);
                        st.push({pp, i});
                        used = true;
                    } else {
                        Index ii = res.size(); 
                        st.push({pp, ii});
                        res.emplace_back(res[i].begin(), res[i].end()-1);
                        res[ii].push_back(pp);
                    }
                }
            } 
        }
        for (auto& r : res) {
            reverse(r.begin(), r.end());
        }
        return res;
    }
    
    
    
    bool is_neighbor(const Position& p, Direction dir) {
        return (*is_neighbor_)(p, dir);
    }
    
    // so i wouldn't go back or override good values with bad onces
    Grid<short> distance_;
    Grid<std::array<bool, 4>> from_;
    const IsNeighbor* is_neighbor_;
    const Grid<T>* g_;
};




}
}