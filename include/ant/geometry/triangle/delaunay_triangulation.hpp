//
//  delaunay_triangulation.hpp
//  ANT
//
//  Created by Anton Logunov on 6/18/15.
//
//

#pragma once

/// probably we are going to work with integers here
// to prevent from (is point on the line debate)

// need my paper with instructions



template<>
class DelaunayTriangulation {

    PointLocation point_location_;

    // first 3 points to ise
    // should i hide them somehow
    void Compute(vector<Point> ps, Point p_0, p_1, p_2) {
        // first add everything in point location
        for (auto& p : ps) {
            Triangle& tr = Find(p); 
            if (tr.IsInside(p)) {
                // divide triangle on three pieces
                LegalizeEdge() // inside new triangle
                
            } else {
                // if lies on an edge...
                // ... we do a lot of stuff
            }
            
        }
        // discarding p_1, p_2 with incident edges and p_0 too...

    }

    Triangle& Find(const Point& p) {
        
    }

    void LegalizeEdge(const Index r, Edge e_ij) {
        if (IsEdgeIllegal(e_ij)) {
            k - find triangle adj to given.. and this is vertex is 'opposite'
            new_edge = pl.Flip(e_ij);
            LegalizeEdge(r, {e_ij[0],k})
            LegalizeEdge(r, {e_ij[1],k})
        }
    }

    bool IsEdgeIllegal(Edge e) {
        return true;
    }

};

