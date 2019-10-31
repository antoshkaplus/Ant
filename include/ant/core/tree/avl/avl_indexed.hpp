#pragma once

#include "avl_tree_indexed.hpp"


namespace ant::core::tree::avl {

template<class T_>
class AVL_Indexed {
    using T = T_;
    typename base::UN<base::AVL_NodeIndexed<T>> root;
public:

    using ConstIterator = BST_IteratorStack<const base::AVL_NodeIndexed<T>>;
    using Iterator = BST_IteratorStack<base::AVL_NodeIndexed<T>>;

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void InsertAt(Index index, T value) {
        base::InsertAt( root, index, value );
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void RemoveAt(Index index) {
        base::RemoveAt( root, index );
    }

    T& operator[](Index index) {
        return base::At(root, index);
    }

    const T& operator[](Index index) const {
        return base::At(root, index);
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return base::Size(root.get());
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