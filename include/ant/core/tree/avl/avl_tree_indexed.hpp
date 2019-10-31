#pragma once

#include "avl_tree.hpp"


namespace ant::core::tree::avl::base {

template <typename T>
struct AVL_NodeIndexed {
    using ValueType = T;

    T value_;
    Count size = 0;
    Count height;
    std::array<std::unique_ptr<AVL_NodeIndexed>, 2> children;

    AVL_NodeIndexed() = default;
    AVL_NodeIndexed(T value) : value_(value), size(1) {}
    AVL_NodeIndexed(T value, std::unique_ptr<AVL_NodeIndexed>&& left_child, std::unique_ptr<AVL_NodeIndexed>&& right_child)
            : value_(value) {

        children[0] = std::move(left_child);
        children[1] = std::move(right_child);
    }

    T& value() { return value_; }
    const T& value() const { return value_; }

    void Update() {
        height = std::max(Height(children[0]), Height(children[1])) + 1;
        size = Size(children[0]) + Size(children[1]) + 1;
    }
};

template <typename Node>
int Size(const UN<Node>& ptr) {
    return ptr ? ptr->size : 0;
}

/* Inserts the given key in the given tree
 * and adjust it so that it continues to be an AVL tree.
 * tree->h is increased by at most one.
 */
template <typename Node>
void InsertAt(UN<Node>& tree, Index index, typename Node::ValueType value) {
    if(!tree) {
        tree = std::make_unique<Node>(value);
    }
    else if(index < Size(Left(tree.get()))) {
        InsertAt(tree->children[0], index, value);
    }
    else if(index > Size(Left(tree.get()))) {
        index -= Size(Left(tree.get()))+1;
        InsertAt(tree->children[1], index, value);
    }
    else {
        std::swap(value, tree->value_);
        InsertAt(tree->children[1], 0, value);
    }
    FixAvl(tree);
}

/* Removes the given key from the tree.
 */
template <typename Node>
void RemoveAt(UN<Node>& tree, Index index) {
    if(!tree) return;
    if(index < Size(Left(tree.get()))) {
        RemoveAt(tree->children[0], index);
    }
    else if(index > Size(Left(tree.get()))) {
        index -= Size(Left(tree.get()))+1;
        RemoveAt(tree->children[1], index);
    }
    else {
        // Key is here.
        if(!tree->children[0]) {
            tree = std::move(tree->children[1]);
            return;
        }
        UN<Node> tmp;
        RemoveMax(tree->children[0], tmp);
        for (int i : {0, 1}) {
            tmp->children[i] = std::move(tree->children[i]);
        }
        tree = std::move(tmp);
    }
    FixAvl(tree);
}

template <typename Node>
typename Node::ValueType& At(UN<Node>& tree, Index index) {
    if(!tree) {
        throw std::runtime_error("out of range");
    }
    else if(index < Size(Left(tree.get()))) {
        return At(tree->children[0], index);
    }
    else if(index > Size(Left(tree.get()))) {
        index -= Size(Left(tree.get()))+1;
        return At(tree->children[1], index);
    }
    else {
        return tree->value_;;
    }
}

template <typename Node>
int Size(const Node* ptr) {
    return ptr != nullptr ? ptr->size : 0;
}

}