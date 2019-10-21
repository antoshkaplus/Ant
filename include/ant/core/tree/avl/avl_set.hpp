#pragma once

#include "avl_tree.hpp"
#include "../bst_iterator_stack.hpp"

namespace ant::core::tree::avl {

template<class T_>
class AVL_Set {
    using T = const T_;
    typename AVL_Base<T>::UN root;
public:

    using ConstIterator = BST_IteratorStack<const typename AVL_Base<T>::Node>;

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(T key) {
        return AVL_Base<T>::Contains(root, key)? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(T key) {
        AVL_Base<T>::Insert( root, key );
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(T key) {
        AVL_Base<T>::Remove( root, key );
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return BST<typename AVL_Base<T>::Node>::Size(root.get());
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