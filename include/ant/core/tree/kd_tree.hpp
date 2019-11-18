// https://en.wikipedia.org/wiki/K-d_tree
// http://www.pointclouds.org/documentation/tutorials/kdtree_search.php
// https://gopalcdas.com/2017/05/24/construction-of-k-d-tree-and-using-it-for-nearest-neighbour-search/

#pragma once

#include "ant/core/core.hpp"
#include "ant/core/bst.hpp"

namespace ant::core::tree::kd {

// TODO Compare with CGAL implementation

// should  be around 2^dimension_count datapoints

// what if each dimension has different types
// never encountered it.

template <typename Value>
struct Edge {
    Value origin;
    Value length;
};

// another metric requires another way of computing distance from point to the box

template <typename Value, ant::Count dimension_count>
struct Box {
    std::array<Edge, dimension_count> edges;

    typename RandomAccessIterator
    Box(RandomAccessIterator begin, RandomAccessIterator end) {
        for (auto axis = 0; axis < dimension_count; ++axis) {
            auto comp = [&](auto a_1, auto a_2) {
                return axis_getter(axis) < axis_getter(axis);
            };

            edges[axis].origin = *std::min_element(begin, end, comp);
            edges[axis].length = *std::max_element(begin, end, comp) - edges[axis].origin;
        }
    }

    Value min(ant::Count axis) {
        return edges[axis].origin;
    }

    Value max(ant::Count axis) {
        return  edges[axis].origin + edges[axis].length;
    }
};

template <typename Value, ant::Count dimension_count>
struct EuclidMetric {

    template <typename Point, typename AxisGetter>
    auto Distance(Point p_1, Point p_2, AxisGetter&& axis_getter) {
        Value value {};
        for (auto axis = 0; axis < dimension_count; ++axis) {
            daxis = axis_getter(p_1, axis) - axis_getter(p_2, axis);
            value += daxis * daxis;
        }
        return value;
    }

    template <typename Point, typename AxisGetter>
    auto Distance(Point p, const Box<Value, dimension_count>& box, AxisGetter&& axis_getter) {
        Value value {};
        for (auto axis = 0; axis < dimension_count; ++axis) {
            daxis = std::max(Value{}, std::max(box.min(axis) - axis_getter(p, axis), axis_getter(p, axis) - box.max(axis)));
            value += daxis * daxis;
        }
        return value;
    }

};

template <typename Point_, ant::Count dimension_count_>
struct Node {
    constexpr static ant::Count dimension_count = dimension_count_;
    using Point = Point_;

    Point point;
    std::array<std::unique_ptr<Node>, 2> children;

};

template <typename Node, typename RandomAccessIterator, typename AxisGetter>
std::unique_ptr<Node> Build(RandomAccessIterator begin, RandomAccessIterator end, AxisGetter&& axis_getter) {
    std::unique_ptr<Node> root;
    Build(root, begin, end, axis_getter, 0);
    return std::move(root);
}

// could probably just keep in array if doesn't change
template <typename Node, typename RandomAccessIterator, typename AxisGetter>
void Build(std::unique_ptr<Node>& node, RandomAccessIterator begin, RandomAccessIterator end, AxisGetter&& axis_getter, Index depth) {
    if (end - begin == 0) return;

    auto axis = depth % Node::dimension_count;
    auto median = (end - begin + 1) / 2;

    std::nth_element(begin, begin + median, end, [&](auto& s_1, auto& s_2) {
        return axis_getter();
    });

    node = std::make_unique(*(begin + median));
    Build(node->children[0], begin, begin + median, axis_getter, depth + 1);
    Build(node->children[1], begin + median + 1, end, axis_getter, depth + 1);
}

// could use reference instead
template <typename Point, typename Distance>
struct NearestNeighbourResult {
    Point point;
    Distance distance;
};

template <typename Node, typename AxisGetter, typename DistanceGetter>
std::optional<NearestNeighbourResult> NearestNeighbour(
        std::unique_ptr<Node>& node, AxisGetter&& axis_getter,
        DistanceGetter&& distance, Box& box, Index depth) {

    using Distance = decltype(DistanceGetter(std::declval<Node::Point>(), std::declval<Node::Point>()));

    if (!node) return {};

    std::optional<NearestNeighbourResult> best;
    // need corresponding box
    Box box;
    // don't think should return anything
    if (best && Distance(query_point, box) >= best.distance) return {};

    if (Distance(node->point, query_point) < best.distance) {
        best = {node->point, Distance(node->point, query_point)};
    }

    auto query_axis = axis_getter(query_point);
    auto node_axis = axis_getter(node->point);

    auto visit_first = 0;
    if (query_axis > node_axis) {
        visit_first = 1;
    }

    auto
    // UpdateBox
    NearestNeighbour(node->children[visit_first], query_point, axis_getter, distance, depth + 1);
    // UpdateBox
    NearestNeighbour(node->children[kAnother[visit_first]], query_point, axis_getter, distance, depth + 1);
    // UpdateBox back
}

}