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

    // create using base node

    using PL = DelaunayPointLocation<IsInsideType, IsOnSegmentType, ToCircumCircleType, DistanceType>;
    using BASE_PL = PointLocation<IsInsideType, IsOnSegmentType>;
    
    using typename BASE_PL::NodeIndex;
    
    // destructor should be virtual
    class Node : public BASE_PL::Node {
    public:
        d2::f::Circle circle;
        
        Node(const Triangle& trg, const PL& pl) : BASE_PL::Node(trg) {
            circle = pl.ToCircumCircle(trg);
        }
    
        virtual ~Node() {}
    }; 
    
    virtual typename BASE_PL::Node* NewLeafNode(const Triangle& trg) override {
        return new Node(trg, *this);
    } 
    
    
    
    virtual void PostInsertInside(Index i, const Triangle& trg) override {
        for (auto& e : trg.Edges()) {
            LegalizeEdge(i, e);
        }
    }
    
    virtual void PostInsertOnEdge(Index i, const Edge& e) override {
        auto nnns = { BASE_PL::neighbors().at({e[0], i}), BASE_PL::neighbors().at({e[1], i}) };
        for (auto& ns : nnns) {
            for (auto& n : ns) {
                auto ee = BASE_PL::nodes()[n]->trg.OppositeEdge(i);
                if (ee == e) continue;
                LegalizeEdge(i, ee);
            }
        }
    }
    
    void LegalizeEdge(const Index r, Edge e_ij) {
        if (!IsEdgeLegal(e_ij)) {
            auto& ns = BASE_PL::neighbors().at(e_ij);
            auto k_0 = BASE_PL::nodes()[ns[0]]->trg.Third(e_ij);
            auto k_1 = BASE_PL::nodes()[ns[1]]->trg.Third(e_ij);
            Index k = r == k_0 ? k_1 : k_0;
            BASE_PL::Flip(e_ij);
            LegalizeEdge(r, {e_ij[0],k});
            LegalizeEdge(r, {e_ij[1],k});
        }
    }
    
    bool IsEdgeLegal(Edge e) {
        auto ns = BASE_PL::neighbors().at(e);
        if (ns[0] == -1 || ns[1] == -1) return true;
        auto n_0 = static_cast<Node*>(BASE_PL::nodes()[ns[0]]);
        auto n_1 = BASE_PL::nodes()[ns[1]];
        return Distance(n_0->circle, n_1->trg.Third(e)) > n_0->circle.radius;
    }

public:
    // need to init a lot of stuff
    // or provide setters
    DelaunayPointLocation(Count ps_count, 
                          const IsInsideType& is_inside, 
                          const IsOnSegmentType& is_on_segment, 
                          const ToCircumCircleType& to_circum_circle, 
                          const DistanceType& distance) 
        : BASE_PL(ps_count, is_inside, is_on_segment) {
        
        distance_ = &distance;
        to_circum_circle_ = &to_circum_circle;
    }
    
    double Distance(const d2::f::Circle& c, Index i) const {
        return (*distance_)(c, i);
    }
    
    d2::f::Circle ToCircumCircle(const Triangle& trg) const {
        return (*to_circum_circle_)(trg);
    }
    
    const DistanceType* distance_;
    const ToCircumCircleType* to_circum_circle_;
};



}
}
}