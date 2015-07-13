//
//  naive_triangulation.hpp
//  Ant
//
//  Created by Anton Logunov on 7/13/15.
//
//

#ifndef Ant_naive_triangulation_hpp
#define Ant_naive_triangulation_hpp

#include "point_location.hpp"

namespace ant {
namespace geometry {
namespace triangle {

    
class NaiveTriangulation {
    
    // return triangle indices
    std::vector<Triangle> Compute(vector<d2::i::Point> ps, d2::i::Triangle& tr) {
        Count sz = ps.size();
        ps.insert(ps.end(), tr.begin(), tr.end());
        
        auto is_inside = [&](Index index, const Triangle& tr) {
            return d2::PointTriangle<d2::i::Point, Int>(ps[tr[0]], ps[tr[1]], ps[tr[2]]).IsInsideOrLies(ps[index]);
        };
        auto is_on_segment = [&](Index v, Index s_0, Index s_1) {
            return d2::i::Segment(ps[s_0], ps[s_1]).Lie(ps[v]);
        };
        
        Triangle tt{sz, sz+1, sz+2};
        PointLocation<decltype(is_inside), decltype(is_on_segment)> pl(tt, ps.size(), is_inside, is_on_segment);
        
        // should randomize this insertion
        for (Index i = 0; i < sz; ++i) {
            pl.Insert(i);
        }
        
        // should remove excessive points from ps vector
        return pl.Triangles();
    }
    
    
};


}
}
}


#endif
