#pragma once

#include "avl_tree.hpp"

namespace ant::core::tree::avl {

template<typename Key, typename Value>
class AVL_Map {
    using NodeValue = std::pair<const Key, Value>;

    typename base::UN<base::AVL_Node<NodeValue>> root;

    struct Compare {
        bool operator()(const NodeValue& nv, const Key& key) const {
            return nv.first < key;
        }

        bool operator()(const Key& key, const NodeValue& nv) const {
            return key < nv.first;
        }

        bool operator()(const NodeValue& nv_1, const NodeValue& nv_2) const {
            return nv_1.first < nv_2.first;
        }
    };

public:
    using ConstIterator = BST_IteratorStack<const typename base::AVL_Node<NodeValue>>;
    using Iterator = BST_IteratorStack<typename base::AVL_Node<NodeValue>>;

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(Key key) {
        return base::Contains(root, key, Compare())? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(const NodeValue& nodeValue) {
        base::Insert(root, base::ParamsCompare<const Compare>{Compare()}, nodeValue);
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(Key key) {
        base::Remove( root, key, Compare() );
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return BST<typename base::AVL_Node<NodeValue>>::Size(root.get());
    }

    ConstIterator begin() const {
        if (root) return ConstIterator(root.get());
        return end();
    }

    ConstIterator end() const {
        return ConstIterator();
    }

    Iterator begin() {
        if (root) return Iterator(root.get());
        return end();
    }

    Iterator end() {
        return Iterator();
    }
};

}