#pragma once

#include "ant/core/core.hpp"
#include "ant/core/tree/bst.hpp"
#include "ant/core/tree/bst_iterator.hpp"


namespace ant::core::tree::avl::base {

template <typename T>
struct AVL_NodeReduce {
    using ValueType = T;

    T value_;
    T opRes;
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

    void Update() {
        height = std::max(Height(children[0]), Height(children[1])) + 1;
        size = Size(children[0]) + Size(children[1]) + 1;
        opRes = value_;
        if (children[0]) opRes = op(opRes, children[0]->opRes);
        if (children[1]) opRes = op(opRes, children[1]->opRes);
    }
};

}