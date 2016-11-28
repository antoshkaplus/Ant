//
//  naive_triangulation.hpp
//  Ant
//
//  Created by Anton Logunov on 7/13/15.
//
//

#ifndef Ant_naive_triangulation_hpp
#define Ant_naive_triangulation_hpp

#include <iostream>

#include "delaunay_point_location.hpp"


// should be exposed to d2::i probably
namespace ant {
namespace geometry {
namespace triangle {

    
class NaiveTriangulation {
     std::vector<d2::f::Point> ps_f;
    
public:
    // return triangle indices
    std::vector<Triangle> Compute(std::vector<d2::i::Point> ps, d2::i::Triangle& tr) {
        Count sz = ps.size();
        ps.insert(ps.end(), tr.begin(), tr.end());
        
        auto is_inside = [&](Index index, const Triangle& tr) {
            return d2::PointTriangle<d2::i::Point, Int>(ps[tr[0]], ps[tr[1]], ps[tr[2]]).IsInsideOrLies(ps[index]);
        };
        auto is_on_segment = [&](Index v, Index s_0, Index s_1) {
            return d2::i::Segment(ps[s_0], ps[s_1]).Lie(ps[v]);
        };
        
        Triangle tt{sz, sz+1, sz+2};
        PointLocation<decltype(is_inside), decltype(is_on_segment)> pl(ps.size(), is_inside, is_on_segment);
        pl.Init(tt);
        
        // should randomize this insertion
        for (Index i = 0; i < sz; ++i) {
            pl.Insert(i);
        }
        ps_f.resize(ps.size());
        std::transform(ps.begin(), ps.end(), ps_f.begin(), [](const d2::i::Point& p) {
            return d2::f::Point(p.x, p.y); 
        });
        
        Legalize(pl);
        pl.PrintNodes(std::cout);
        pl.PrintLeafNodes(std::cout);
        
        ps.erase(ps.end()-3, ps.end());
        // should remove excessive points from ps vector
        auto trgs = pl.Triangles();
//        Index I = ps.size();
//        auto cond = [=](const Triangle& t) {
//            for (Index i = 0; i < 3; ++i) {
//                if (t[i] >= I) return true;
//            }
//            return false;
//        };
//        trgs.erase(std::remove_if(trgs.begin(), trgs.end(), cond), trgs.end());
        return trgs;
    }
    
    template<class PL>
    void CheckNeighbors(const PL& pl) {
        for (auto& p : pl.neighbors()) {
            if (!(((p.second[0] >= 0 && p.second[0] <= 100) || p.second[0] == -1) &&
                  ((p.second[1] >= 0 && p.second[1] <= 100) || p.second[1] == -1) )) {
                
                assert(false);
            }
        }
    }
    
    template<class PL> 
    void CheckTriangles(const PL& pl) {
        auto trs = pl.Triangles();
        for (Index i = 0; i < trs.size(); ++i) {
            for (Index j = i+1; j < trs.size(); ++j) {
                assert(trs[i] != trs[j]);
            }
        }
    }
    
    
    template<class PL>
    void Legalize(PL& pl) {
        // edge, pair or indices of triangles
        bool flipped = true;
        CheckTriangles(pl);
        while (flipped) {
            flipped = false;
            // flip is going to remove edge, 
            // so i need to save it somewhere
            auto& neighbors = pl.neighbors(); 
            auto it = neighbors.begin();
            while (it != neighbors.end()) {
                const Edge& e = it->first;
                Index t_0 = it->second[0];
                Index t_1 = it->second[1];
                ++it;

                if (t_0 == -1 || t_1 == -1) {
                    continue;
                }
                auto& tt_0 = pl.triangle(t_0);
                auto& tt_1 = pl.triangle(t_1); 
                Index i_0 = tt_0.Third(e);
                Index i_1 = tt_1.Third(e);
                
                d2::f::Circle c = geometry::d2::f::CircumCircle(ps_f[e[0]], ps_f[e[1]], ps_f[i_0]);
                if (c.IsInside(ps_f[i_1])) {
                    pl.Flip(e);
                    pl.PrintNodes(std::cout);
                    CheckTriangles(pl);
                    CheckNeighbors(pl);
                    flipped = true;
                }
            }
        }
    }
    
    
};



}
}
}


#endif
