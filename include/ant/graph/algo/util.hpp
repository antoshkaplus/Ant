#pragma once

namespace ant::graph {

template <typename Graph>
ant::Count CountVertices(Graph& g) {
    return std::count_if(g.vertices().begin(), g.vertices().end(), [](const auto&){ return true; });
}


}