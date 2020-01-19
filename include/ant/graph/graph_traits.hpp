#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {

template<typename Graph, typename = void>
struct has_vertex_descriptor_index : std::false_type {
};
template<typename Graph>
struct has_vertex_descriptor_index<Graph, std::void_t<decltype(&Graph::vertex_descriptor_index)>> : std::true_type {
};

template<typename Graph, bool has_directed = has_vertex_descriptor_index<Graph>::value>
struct is_vertex_descriptor_index;
template<typename Graph>
struct is_vertex_descriptor_index<Graph, true> {
    constexpr static bool value = Graph::vertex_descriptor_index;
};
template<typename Graph>
struct is_vertex_descriptor_index<Graph, false> {
    constexpr static bool value = false;
};
template<typename Graph>
inline constexpr bool is_vertex_descriptor_index_v = is_vertex_descriptor_index<Graph>::value;


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


template<typename Graph, class = std::void_t<>>
struct is_vertex_value : std::false_type {};
template<typename Graph>
struct is_vertex_value<Graph, std::void_t<decltype( Graph::VertexValue )>> : std::true_type {};
template<typename Graph>
inline constexpr bool is_vertex_value_v = is_vertex_value<Graph>::value;

template<typename Graph, class = std::void_t<>>
struct is_edge_descriptor : std::false_type {};
template<typename Graph>
struct is_edge_descriptor<Graph, std::void_t<decltype( Graph::EdgeDescriptor )>> : std::true_type {};
template<typename Graph>
inline constexpr bool is_edge_descriptor_v = is_edge_descriptor<Graph>::value;

}