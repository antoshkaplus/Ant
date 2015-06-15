//
//  circle_collision_system.hpp
//  ANT
//
//  Created by Anton Logunov on 6/12/15.
//
//

#ifndef ANT_circle_collision_system_hpp
#define ANT_circle_collision_system_hpp

#include "ant/geometry/d2.hpp"
#include "ant/grid.hpp"


namespace physics {

using namespace ant::geometry::d2::f;
using namespace ant::grid;    

/// Circle methods:
///     mass()
///     radius()
///     center(), set_center()
/// Circle object should be pointer like
template<class CirclePtr>
class CircleCollisionSystem {
    
    // should keep them as pointers or similar.
    // user decides.
    ParticleGrid<CirclePtr> grid_;
    std::vector<CirclePtr>* circles_;
    
    
public:
        
    CircleCollisionSystem(std::vector<CirclePtr>& cs) {
    
    }



    /// user may add it's own collision events...
    class Event {
        void Handle() = 0;
    };
    
    class WallCollision : public Event {
        // one circle
    };
    
    class CircleCollision : public Event {
        // two circles
    
    };
    
    class BorderCrossing : public Event {
        // one circle
    };
    
    

};


}


#endif
