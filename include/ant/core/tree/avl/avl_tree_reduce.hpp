#pragma once

#include "ant/core/tree/avl/avl_tree_indexed.hpp"

namespace ant::core::tree::avl::base {

template <typename T>
struct AVL_NodeReduce {
    using ValueType = T;

    T value_;
    std::remove_const_t<T> opRes;
    Count size;
    Count height;
    std::array<std::unique_ptr<AVL_NodeReduce>, 2> children;

    AVL_NodeReduce() = default;
    AVL_NodeReduce(T value) : value_(value) {}
    AVL_NodeReduce(T value, std::unique_ptr<AVL_NodeReduce>&& left_child, std::unique_ptr<AVL_NodeReduce>&& right_child)
            : value_(value) {

        children[0] = std::move(left_child);
        children[1] = std::move(right_child);
    }

    T& value() { return value_; }
    const T& value() const { return value_; }

    template <typename Op>
    void Update(Op& op) {
        height = std::max(Height(children[0]), Height(children[1])) + 1;
        size = Size(children[0]) + Size(children[1]) + 1;
        opRes = value_;
        if (children[0]) opRes = op(opRes, children[0]->opRes);
        if (children[1]) opRes = op(opRes, children[1]->opRes);
    }
};

template <typename Op>
struct ParamsReduce {
    constexpr static bool has_compare = false;

    Op& op;

    ParamsReduce(Op& op) : op(op) {}

    template <typename Node>
    void update(UN<Node>& node) const {
        node->Update(op);
    }
};

template <typename Node, typename Op>
typename Node::ValueType Reduce(const UN<Node>& tree, Op& op, ant::Index pos, ant::Count count) {
    if (!tree) throw std::out_of_range("");

    auto add = [&](std::optional<std::remove_const_t<typename Node::ValueType>> & v_to, typename Node::ValueType& from) {
        if (v_to) v_to = from;
        else v_to.value() = op(v_to.value(), from);
    };

    std::optional<std::remove_const_t<typename Node::ValueType>> result {};
    if (pos < Size(tree->children[0])) {
        add(result, Reduce(tree->children[0], op, pos, count));

        count -= Size(tree->children[0]) - pos;
        pos = 0;
    } else {
        pos -= Size(tree->children[0]);
    }

    if (pos == 0 && count > 0) {
        add(result, tree->value());

        pos = 0;
        count -= 1;
    } else {
        pos -= 1;
    }

    if (count > 0) {
        add(result, Reduce(tree->children[1], op, pos, count));
    }

    if (result) return result.value();

    throw std::out_of_range("");
}

}