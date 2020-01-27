#pragma once

namespace ant::graph {

template <typename Graph>
ant::Count CountVertices(const Graph& g) {
    return std::count_if(g.verices().begin(), g.vertices().end(), [](const auto&){ return true; });
}


}