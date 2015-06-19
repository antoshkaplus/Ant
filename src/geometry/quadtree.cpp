//
//  quadtree.cpp
//  ANT
//
//  Created by Anton Logunov on 6/19/15.
//
//

#include "ant/geometry/quadtree.hpp"

namespace ant {
    
namespace geometry {

namespace d2 {
    
namespace f {

void Quadtree::Node::Insert(const Point& p, 
                            const Data& data, 
                            Node** node) {
    *node = new ValueNode<VALUE_NODE_CAP>();
    (**node).Insert(p, data, node);
    delete this;
}


}
    
}
    
}

}
