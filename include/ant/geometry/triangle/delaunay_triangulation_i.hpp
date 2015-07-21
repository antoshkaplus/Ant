
#pragma once

#include <algorithm>

#include "ant/geometry/triangle/delaunay_point_location.hpp"


namespace ant {
namespace geometry {
namespace d2 {
namespace i {

    
// may need to have thing to returns...
class DelaunayTriangulation {
    
    
public:
    std::vector<triangle::Triangle> Compute(vector<d2::i::Point> ps, d2::i::Triangle& tr) {
        Count sz = ps.size();
        ps.insert(ps.end(), tr.begin(), tr.end());
        
        auto is_inside = [&](Index index, const triangle::Triangle& tr) {
            return d2::PointTriangle<d2::i::Point, Int>(ps[tr[0]], ps[tr[1]], ps[tr[2]]).IsInsideOrLies(ps[index]);
        };
        auto is_on_segment = [&](Index v, Index s_0, Index s_1) {
            return d2::i::Segment(ps[s_0], ps[s_1]).Lie(ps[v]);
        };
        
        // need to return circle
        auto to_circum_circle = [&](const triangle::Triangle& trg) {
            std::array<d2::f::Point, 3> trg_f;
            for (auto i = 0; i < 3; ++i) {
                trg_f[i].set(ps[trg[i]].x, ps[trg[i]].y);
            }
            return CircumCircle(trg_f); 
        }; 
        
        auto distance = [&](const d2::f::Circle& c, Index i) {
            return c.center.Distance(d2::f::Point{double(ps[i].x), double(ps[i].y)});
        };
        
        triangle::Triangle tt{sz, sz+1, sz+2};
        using f_0 = decltype(is_inside);
        using f_1 = decltype(is_on_segment);
        using f_2 = decltype(to_circum_circle);
        using f_3 = decltype(distance);
        triangle::DelaunayPointLocation<f_0, f_1, f_2, f_3> pl(ps.size(), is_inside, is_on_segment, to_circum_circle, distance);
        pl.Init(tt);
        
        // should randomize this insertion
        for (Index i = 0; i < sz; ++i) {
            pl.Insert(i);
        }
        ps.erase(ps.end()-3, ps.end());
        auto trgs = pl.Triangles();
        auto fr = [=](const triangle::Triangle& t) {
            return t[0] >= sz || t[1] >= sz || t[2] >= sz;
        };
        trgs.erase(std::remove_if(trgs.begin(), trgs.end(), fr), trgs.end());
        return trgs;
    }
    
};

    
}  
} 
}
}


