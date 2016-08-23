//
//  segment_grid.hpp
//  Ant
//
//  Created by Anton Logunov on 4/14/16.
//
//

#pragma once

#include "ant/geometry/d2.hpp"
#include "ant/grid/grid.hpp"

namespace ant {
namespace geometry {
namespace d2 {
namespace f {
  
    
class SegmentGrid {

    struct S {
        Id id;
        Segment segment;
    };
public:
    
    
    // we divide by 2 if part in one ce and another in another.
    // we need to divide even more if segment takes multiple cells
    
    // should better return just id back
    std::vector<Id> Add(const Segment& s) {
        
        
        return std::vector<Id>();
    }
    
    Float Length(Id segmentId) {
        return 0;
    }
    
    
    
private:
    /*
    ent
    
    // i can store unordered map or vector
    // small set vector should be just fine
    grid::Grid<std::vector<S>> segments_;
    std::unordered_map<Id, grid::Position> positions_;
    
    */
};    
      
          
} 
          
}

}
    
}
