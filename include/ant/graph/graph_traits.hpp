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

// has is good when you have some default well established treatment
// otherwise should be copilation error is not there.
template<typename Graph, class = std::void_t<>>
struct has_directed : std::false_type {};

template<typename Graph>
struct has_directed<Graph, std::void_t<decltype( Graph::directed )>> : std::true_type {};

template<typename Graph, bool has_directed = has_directed<Graph>::value>
struct is_directed;

template<typename Graph>
struct is_directed<Graph, true> {
    constexpr static bool value = Graph::directed;
};
template<typename Graph>
struct is_directed<Graph, false> {
    constexpr static bool value = false;
};
template<typename Graph>
inline constexpr bool is_directed_v = is_directed<Graph>::value;

}