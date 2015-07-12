
#include "ant/geometry/triangle/triangle.hpp"

namespace ant {
    
namespace geometry {
        
namespace triangle {
            
std::ostream& operator<< (std::ostream& o, const Triangle& triag) {
    return o << "Triangle: " << triag[0] << " " << triag[1] << " " << triag[2] << std::endl;
}
 
}

}

}