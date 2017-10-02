#pragma once


#include <iostream>
#include <array>
#include <map>

#include "d2.hpp"


namespace ant::geometry::d2 {

// we keep segments in the Index vector

template<typename Point, typename Intersect>
class BentleyOttmann {

    // sweep line statuses
    std::multimap<double, Index> status;
    std::priority_queue<Point> events;

    // provide back segment that intersect
    // need to think more about data structure with better locality
    // do I have it already???
    NestedVectors<Point> FindIntersections(const std::vector<Point>& ps,
                                           const std::vector<P>& segs,
                                           const Intersect& inter) {



    }

};