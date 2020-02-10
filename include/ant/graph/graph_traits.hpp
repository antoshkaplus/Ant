#pragma once

#include <ant/core/core.hpp>

namespace ant::graph {

#define ANT_TRAIT_HAS_MEMBER_VAR_2(trait_name, member_var_name)             \
    template <typename Type, typename = void>           \
    struct has_##trait_name : std::false_type {};  \
    template <typename Type>                            \
    struct has_##trait_name<Type, std::void_t<decltype(&Type::member_var_name)>> : std::true_type {};

#define ANT_TRAIT_HAS_MEMBER_VAR_1(member_var_name) ANT_TRAIT_HAS_MEMBER_VAR_2(member_var_name, member_var_name)


#define ANT_TRAIT_IS_MEMBER_TYPE_2(trait_name, member_type_name)   \
    template<typename Type, class = std::void_t<>> \
    struct is_##trait_name : std::false_type {}; \
    template<typename Type> \
    struct is_##trait_name<Type, std::void_t<typename Type::member_type_name>> : std::true_type {}; \
    template<typename Type> \
    inline constexpr bool is_##trait_name##_v = is_##trait_name<Type>::value;


ANT_TRAIT_HAS_MEMBER_VAR_1(vertex_descriptor_index)
ANT_TRAIT_IS_MEMBER_TYPE_2(edge_descriptor, EdgeDescriptor)



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
struct has_vertex_value : std::false_type {};
template<typename Graph>
struct has_vertex_value<Graph, std::void_t<typename Graph::VertexValue>> : std::true_type {};

template<typename Graph, bool has_vertex_value = has_vertex_value<Graph>::value>
struct is_vertex_value;
template<typename Graph>
struct is_vertex_value<Graph, true> {
    constexpr static bool value = !std::is_void<typename Graph::VertexValue>::value;
};
template<typename Graph>
struct is_vertex_value<Graph, false> {
    constexpr static bool value = false;
};
template<typename Graph>
inline constexpr bool is_vertex_value_v = is_vertex_value<Graph>::value;






template<typename Graph, class = std::void_t<>>
struct has_edge_descriptor_index : std::false_type {};
template<typename Graph>
struct has_edge_descriptor_index<Graph, std::void_t<decltype(Graph::edge_descriptor_index)>> : std::true_type {};

template<typename Graph, bool has_directed = has_edge_descriptor_index<Graph>::value>
struct is_edge_descriptor_index;
template<typename Graph>
struct is_edge_descriptor_index<Graph, true> {
    constexpr static bool value = Graph::edge_descriptor_index;
};
template<typename Graph>
struct is_edge_descriptor_index<Graph, false> {
    constexpr static bool value = false;
};
template<typename Graph>
inline constexpr bool is_edge_descriptor_index_v = is_edge_descriptor_index<Graph>::value;



}