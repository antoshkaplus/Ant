#pragma once

#include <experimental/optional>
#include <cmath>


namespace ant::linalg {

inline std::experimental::optional<std::array<double, 2>> SolveQuadEq(double a, double b, double c) {
    auto discriminant = b*b - 4*a*c;

    if (discriminant >= 0) {
        auto x_1 = (-b - std::sqrt(discriminant)) / (2 * a);
        auto x_2 = (-b + std::sqrt(discriminant)) / (2 * a);
        return {{{x_1, x_2}}};
    }

    return {};
}


}