
#pragma once


#include "ant/core/core.hpp"
#include "ant/core/sort.hpp"

#include <array>


namespace ant::geometry::triangle {


using V_3 = std::array<Index, 3>;
using V_2 = std::array<Index, 2>;

// vertices are sorted for easy comparison
struct Edge {
    V_2 vs;
    
    Edge() {}
    
    Edge(Index v_0, Index v_1) : vs{v_0, v_1} {
        Sort(vs);
    }
    
    bool Has(Index v) {
        return vs[0] == v || vs[1] == v;
    }
    
    Index operator[](Index i) const {
        return vs[i];
    }
    
    bool operator==(const Edge& e) const {
        return vs == e.vs;
    }
};

inline std::ostream& operator<<(std::ostream& out, const Edge& edge) {
    return out << "Edge: " << edge[0] << ", " << edge[1];
}


struct Triangle {
    V_3 vs;
    
    Triangle() {}
    
    Triangle(Index v_0, Index v_1, Index v_2) : vs{v_0, v_1, v_2} {
        Sort(vs);
    }
    
    Triangle(const Edge& edge, Index v) : Triangle(edge[0], edge[1], v) {}
    
    bool IsVertex(Index v) {
        return Find(vs, v);
    }
    
    // using that tiangle and edge are sorted
    Index Third(const Edge& e) const {
        if (e[0] != vs[0]) {
            return vs[0];
        } else if (e[1] != vs[1]) {
            return vs[1];
        } else return vs[2];
    }
    
    Edge OppositeEdge(Index v) {
        for (Index i = 0; i < 3; ++i) {
            if (vs[i] == v) {
                return {vs[(i+3-1)%3], vs[(i+1)%3]};
            }
        }
        return {-1, -1};
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
};

inline bool operator==(const Triangle& t_0, const Triangle& t_1) {
    return t_0.vs == t_1.vs;
}

inline bool operator!=(const Triangle& t_0, const Triangle& t_1) {
    return !(t_0 == t_1);
}

inline std::ostream& operator<< (std::ostream& o, const Triangle& triag) {
    return o << "Triangle: " << triag[0] << " " << triag[1] << " " << triag[2];
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

