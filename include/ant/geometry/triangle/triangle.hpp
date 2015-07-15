
#pragma once


#include "ant/core/core.hpp"

#include <array>


namespace ant {

namespace geometry {

namespace triangle {



using V_3 = std::array<Index, 3>;
using V_2 = std::array<Index, 2>;

struct Edge {
    V_2 vs;
    
    Edge() {}
    
    Edge(Index v_0, Index v_1) {
        vs[0] = v_0;
        vs[1] = v_1;
        std::sort(vs.begin(), vs.end());
    }
    
    Index operator[](Index i) const {
        return vs[i];
    }
    
    bool operator==(const Edge& e) const {
        return vs == e.vs;
    }
};

struct Triangle {
    V_3 vs;
    
    Triangle() {}
    
    Triangle(Index v_0, Index v_1, Index v_2) {
        vs[0] = v_0;
        vs[1] = v_1;
        vs[2] = v_2;
        std::sort(vs.begin(), vs.end());
    }
    
    Triangle(const Edge& edge, Index v) {
        vs[0] = edge[0];
        vs[1] = edge[1];
        vs[2] = v;
        std::sort(vs.begin(), vs.end());
    }
    
    bool IsVertex(Index v) {
        return std::find(vs.begin(), vs.end(), v) != vs.end();
    }
    
    // using that tiangle and edge are sorted
    Index Third(const Edge& e) const {
        if (e[0] != vs[0]) {
            return vs[0];
        } else if (e[1] != vs[1]) {
            return vs[1];
        } else return vs[2];
    }
    
    const std::array<Edge, 3> Edges() const {
        return {{ {vs[0],vs[1]}, {vs[1],vs[2]}, {vs[0],vs[2]} }};
    }
    
    // should take three Indices as arguments. first - vertex,
    // two other - edge
    template<class IsOnSegment>
    std::pair<Edge, bool> OnEdge(Index v, const IsOnSegment& is_on_segment) const {
        char ch_0[3] = {0, 1, 2};
        char ch_1[3] = {1, 2, 0};
        for (Index i = 0; i < 3; ++i) {
            Index v_0 = vs[ch_0[i]];
            Index v_1 = vs[ch_1[i]];
            if (is_on_segment(v, v_0, v_1)) {
                return {{v_0, v_1}, true};
            }
        }
        std::pair<Edge, bool> r;
        r.second = false;
        return r;  
    }
    
    Index operator[](Index i) const {
        return vs[i];
    }
    
    bool IsInsideOrLie(Index v) const {
        return true;
        // lol
    }
};    

bool operator==(const Triangle& t_0, const Triangle& t_1);
bool operator!=(const Triangle& t_0, const Triangle& t_1);
std::ostream& operator<< (std::ostream& o, const Triangle& triag);


}

}

}

namespace std {
    template <>
    struct hash<ant::geometry::triangle::Edge> {
        size_t operator ()(ant::geometry::triangle::Edge value) const {
            return ant::Hash(value[0], value[1]);
        }
    };
}

