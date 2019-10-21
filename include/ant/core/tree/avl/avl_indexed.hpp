#pragma once

#include "avl_tree_indexed.hpp"


namespace ant::core::tree::avl {

template<class T_>
class AVL_Indexed {
    using T = T_;
    typename AVL_BaseIndexed<T>::UN root;
public:

    using ConstIterator = BST_IteratorStack<const typename AVL_BaseIndexed<T>::Node>;
    using Iterator = BST_IteratorStack<typename AVL_BaseIndexed<T>::Node>;

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void InsertAt(Index index, T value) {
        AVL_BaseIndexed<T>::InsertAt( root, index, value );
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void RemoveAt(Index index) {
        AVL_BaseIndexed<T>::RemoveAt( root, index );
    }

    T& operator[](Index index) {
        return AVL_BaseIndexed<T>::At(root, index);
    }

    const T& operator[](Index index) const {
        return AVL_BaseIndexed<T>::At(root, index);
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return AVL_BaseIndexed<T>::Size(root.get());
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