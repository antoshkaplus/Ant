#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {

template<typename Graph, typename = void>
struct has_vertex_id_index : std::false_type {
};
template<typename Graph>
struct has_vertex_id_index<Graph, std::void_t<decltype(&Graph::vertex_id_index)>> : std::true_type {
};

template<typename Graph>
struct is_vertex_id_index {
    constexpr static bool value = has_vertex_id_index<Graph>::value && Graph::vertex_id_index;
};
template<typename Graph>
inline constexpr bool is_vertex_id_index_v = is_vertex_id_index<Graph>::value;


template<typename Graph, typename = void>
struct has_directed : std::false_type {
};
template<typename Graph>
struct has_directed<Graph, std::void_t<decltype(&Graph::directed)>> : std::true_type {
};

template<typename Graph>
struct is_directed {
    constexpr static bool value = has_directed<Graph>::value && Graph::directed;
};
template<typename Graph>
inline constexpr bool is_directed_v = is_directed<Graph>::value;

}