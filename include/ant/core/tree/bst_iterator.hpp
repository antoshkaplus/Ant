#pragma once

#include <iterator>


namespace ant::core::tree {

template <typename Node>
struct IteratorWrapper;

template <typename Node>
Node* node(IteratorWrapper<Node>& it);

template <typename Node>
struct IteratorWrapper : std::iterator<std::bidirectional_iterator_tag, typename Node::ValueType> {

    IteratorWrapper(Node* node) : current_(node) {}
    IteratorWrapper() {}

    const typename Node::ValueType& operator*() const {
        return current_->value();
    }

    bool operator==(const IteratorWrapper& it) {
        return current_ == it.current_;
    }
    bool operator!=(const IteratorWrapper& it) {
        return current_ != it.current_;
    }

    // pred
    IteratorWrapper& operator++() {
        current_ = BST<Node>::Next(current_);
        return *this;
    }
    // post
    IteratorWrapper operator++(int) {
        IteratorWrapper it(current_);
        current_ = BST<Node>::Next(current_);
        return it;
    }

    // pred
    IteratorWrapper& operator--() {
        // end
        current_ = BST<Node>::Prev(current_);
        return *this;
    }
    // post
    IteratorWrapper operator--(int) {
        IteratorWrapper it(current_);
        current_ = BST<Node>::Prev(current_);
        return it;
    }

private:
    Node* current_;

    friend Node* node<Node>(IteratorWrapper& it);
};

template <typename Node>
Node* node(IteratorWrapper<Node>& it) {
    return it.current_;
}

}