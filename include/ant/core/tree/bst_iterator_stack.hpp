#pragma once

#include <iterator>

#include "bst.hpp"

namespace ant::core::tree {

template <typename Node>
struct BST_IteratorStack : std::iterator<std::forward_iterator_tag, typename Node::ValueType> {

    BST_IteratorStack(Node* root) { GoDown(root); }
    BST_IteratorStack() {}

    const typename Node::ValueType& operator*() const {
        return stack_.top()->value();
    }

    bool operator==(const BST_IteratorStack& it) {
        return stack_ == it.stack_;
    }
    bool operator!=(const BST_IteratorStack& it) {
        return stack_ != it.stack_;
    }

    // pred
    BST_IteratorStack& operator++() {
        auto node = Right(stack_.top());
        stack_.pop();
        if (BST<Node>::Exists(node)) {
            GoDown(node);
        }
        return *this;
    }

private:
    void GoDown(Node* node) {
        stack_.push(node);
        while (BST<Node>::Exists(Left(node))) {
            node = Left(node);
            stack_.push(node);
        }
    }

    std::stack<Node*> stack_;

    friend Node* node(BST_IteratorStack& it);
};

template <typename Node>
Node* node(BST_IteratorStack<Node>& it) {
    return it.stack_.top();
}

}