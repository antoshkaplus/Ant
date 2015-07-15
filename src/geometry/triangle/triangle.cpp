
#include "ant/geometry/triangle/triangle.hpp"

namespace ant {
    
namespace geometry {
        
namespace triangle {



bool operator==(const Triangle& t_0, const Triangle& t_1) {
    return t_0.vs == t_1.vs;
}

bool operator!=(const Triangle& t_0, const Triangle& t_1) {
    return !(t_0 == t_1);
}
            
std::ostream& operator<< (std::ostream& o, const Triangle& triag) {
    return o << "Triangle: " << triag[0] << " " << triag[1] << " " << triag[2] << std::endl;
}
 
 
 
}

}

}