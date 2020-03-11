#pragma once

#include "ant/core/core.hpp"

namespace ant::core::range {

template <typename Range>
auto Count(const Range& range) {
    ant::Count size = 0;
    for (const auto& _ : range) {
        ++size;
    }
    return size;
}

}

#include "filter.hpp"
#include "flatten.hpp"
#include "transform.hpp"