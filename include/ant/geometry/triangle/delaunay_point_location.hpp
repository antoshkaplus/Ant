//
//  delaunay_triangulation.hpp
//  ANT
//
//  Created by Anton Logunov on 6/18/15.
//
//

#pragma once

#include "point_location.hpp"


namespace ant {
namespace geometry {
namespace triangle {
    


template<class IsInsideType, class IsOnSegmentType, class ToCircumCircleType, class DistanceType>
class DelaunayPointLocation : public PointLocation<IsInsideType, IsOnSegmentType> {
protected:

    using PL = DelaunayPointLocation<IsInsideType, IsOnSegmentType, ToCircumCircleType, DistanceType>;
    using BASE_PL = PointLocation<IsInsideType, IsOnSegmentType>;

    class Node : public BASE_PL::Node {
    public:
        d2::f::Circle circle;
        
        Node(const Triangle& trg, const PL& pl) : BASE_PL::Node(trg) {
            circle = pl.ToCircumCircle(trg);
        }
    }; 

    
    virtual void PostInsertInside(Index i, const Triangle& trg) override {
        for (auto& e : trg.Edges()) {
            LegalizeEdge(i, e);
        }
    }
    
    virtual void PostInsertOnEdge(Index i, const Edge& e) override {
        auto ns = BASE_PL::neighbors()[e];
        for (auto& n : ns) {
            for (auto& ee : n->trg.Edges()) {
                if (ee == e) continue;
                LegalizeEdge(i, ee);
            }
        }
    }
    
    void LegalizeEdge(const Index r, Edge e_ij) {
        if (IsEdgeIllegal(e_ij)) {
            auto ns = BASE_PL::neighbors()[e_ij];
            auto k_0 = ns[0]->trg.Third(e_ij);
            auto k_1 = ns[1]->trg.Third(e_ij);
            Index k = r == k_0 ? k_1 : k_0;
            BASE_PL::Flip(e_ij);
            LegalizeEdge(r, {{e_ij[0],k}});
            LegalizeEdge(r, {{e_ij[1],k}});
        }
    }
    
    bool IsEdgeIllegal(Edge e) {
        auto ns = BASE_PL::neighbors()[e];
        return Distance(ns[0]->circle, ns[1]->trg.Third(e)) < ns[0]->circle.radious;
    }

public:
    // need to init a lot of stuff
    // or provide setters
    DelaunayPointLocation() {
    
    }
};


}
}
}
