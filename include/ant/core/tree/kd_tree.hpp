// https://en.wikipedia.org/wiki/K-d_tree
// http://www.pointclouds.org/documentation/tutorials/kdtree_search.php

#pragma once

#include "ant/core/core.hpp"

namespace ant::geometry {

// TODO Compare with CGAL implementation

template <typename Value, ant::Count axis_count>
struct Node {

    Value value;
    std::array<std::unique_ptr<Node>, 2> children;

};

//class Node(namedtuple('Node', 'location left_child right_child')):
//def __repr__(self):
//return pformat(tuple(self))
//
//def kdtree(point_list, depth=0):
//if not point_list:
//return None
//
//        k = len(point_list[0]) # assumes all points have the same dimension
//# Select axis based on depth so that axis cycles through all valid values
//        axis = depth % k
//
//# Sort point list by axis and choose median as pivot element
//point_list.sort(key=itemgetter(axis))
//median = len(point_list) // 2
//
//# Create node and construct subtrees
//return Node(
//        location=point_list[median],
//        left_child=kdtree(point_list[:median], depth + 1),
//right_child=kdtree(point_list[median + 1:], depth + 1)
//)

template <typename Node, typename AxisGetter>
std::unique_ptr<Node> Build( how to pass the point list ) {

}



class KD_Tree {


};


}