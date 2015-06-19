//
//  delaunay_triangulation.hpp
//  ANT
//
//  Created by Anton Logunov on 6/18/15.
//
//

#ifndef ANT_delaunay_triangulation_hpp
#define ANT_delaunay_triangulation_hpp

/// probably we are going to work with integers here
// to prevent from (is point on the line debate)






class DelaunayTriangulation {

    class Triangle {
        Index i_0, i_1, i_2;
    };
    
    class PointLocation {
        
    
    };
    

    // first 3 points to ise
    void Compute(vector<Point> ps, Point p_0, p_1, p_2) {
        
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

    void LegalizeEdge() {
        if (illegal) {
            finding right triangle flip something
            and do legalize step again
            
        }
    }

    

};



#endif
