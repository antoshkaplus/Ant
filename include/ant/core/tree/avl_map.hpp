#pragma once

#include "avl_tree.hpp"

namespace ant::core::tree {

template<typename Key, typename Value>
class AVL_Map {
    using NodeValue = std::pair<Key, Value>;

    typename AVL_Base<NodeValue>::UN root;

public:
    using ConstIterator = BST_IteratorStack<const typename AVL_Base<std::pair<Key, Value>>::Node>;
    using Iterator = BST_IteratorStack<typename AVL_Base<std::pair<const Key, Value>>::Node>;

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(Key key) {
        return AVL_Base<NodeValue>::Contains(root, key)? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(Key key, Value value) {
        AVL_Base<NodeValue>::Insert(root, NodeValue{ key, value });
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(Key key) {
        AVL_Base<NodeValue>::Remove( root, key );
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return BST<typename AVL_Base<NodeValue>::Node>::Size(root.get());
    }

    ConstIterator begin() const {
        if (root) return ConstIterator(root.get());
        return end();
    }

    ConstIterator end() const {
        return ConstIterator();
    }
};

}