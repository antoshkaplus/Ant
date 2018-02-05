#pragma once

#include "ant/geometry/d2.hpp"

namespace ant::geometry::d2::f {

struct Polar {
    double r;
    double phi;
};

inline Polar ToPolar(const Point& p) {
    return {sqrt(p.x*p.x + p.y*p.y), atan2(p.y, p.x)};
}

inline Point FromPolar(const Polar& p) {
    return {p.r * cos(p.phi), p.r * sin(p.phi)};
}

}