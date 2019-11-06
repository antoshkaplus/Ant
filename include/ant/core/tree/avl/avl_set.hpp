#pragma once

#include "avl_tree.hpp"
#include "../bst_iterator_stack.hpp"

namespace ant::core::tree::avl {

template<class T_>
class AVL_Set {
    using T = const T_;
    typename base::UN<base::AVL_Node<T>> root;

    constexpr static base::Params params {};
public:

    using ConstIterator = BST_IteratorStack<const typename base::AVL_Node<T>>;

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(T key) {
        return base::Contains(root, key)? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(T key) {
        base::Insert<base::AVL_Node<T>, const base::Params>( root, params, key );
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(T key) {
        base::Remove<base::AVL_Node<T>, const base::Params>( root, params, key );
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return BST<typename base::AVL_Node<T>>::Size(root.get());
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