#pragma once

#include "d2.hpp"


namespace ant::geometry::d2 {

template<typename Point, typename Segment>
std::vector<std::array<Index, 2>> FindIntersectionPairs(std::vector<Segment>& segs) {
    std::vector<Segment> segs_order_ps(segs.begin(), segs.end());
    std::for_each(segs_order_ps.begin(), segs_order_ps.end(), [](Segment& s) {
        if (s.p_0.x > s.p_1.x) std::swap(s.p_0, s.p_1);
    });
    std::vector<Index> indices(segs.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&](Index i_0, Index i_1) {
        return segs_order_ps[i_0].p_0.x < segs_order_ps[i_1].p_0.x;
    });

    std::vector<std::array<Index, 2>> res;
    for (auto i = 0; i < indices.size(); ++i) {
        for (auto j = i+1; j < indices.size() && segs_order_ps[indices[i]].p_1.x >= segs_order_ps[indices[j]].p_0.x; ++j) {
            if (Intersection(segs_order_ps[indices[i]], segs_order_ps[indices[j]]).second) {
                res.push_back({{indices[i], indices[j]}});
            }
        }
    }
    return res;
};

}