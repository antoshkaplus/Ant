

#include "ant/geometry/d1.hpp"

#include <array>


namespace ant {
    
namespace geometry {

namespace d1 {

    
bool operator==(const Segment& s_0, const Segment& s_1) {
    return s_0.origin == s_1.origin && s_0.length == s_1.length;
}



}

}

}