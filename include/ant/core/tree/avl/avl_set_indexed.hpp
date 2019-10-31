#pragma once

#include "avl_tree.hpp"
#include "../bst_iterator_stack.hpp"

namespace ant::core::tree::avl {

// difference in support of method finding an element index / finding element by index

template<class T_>
class AVL_SetIndexed {
    using T = const T_;
    typename base::UN<base::AVL_NodeIndexed<T>> root;
public:

    using ConstIterator = BST_IteratorStack<const base::AVL_NodeIndexed<T>>;

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(T key) {
        return base::Contains(root, key)? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(T key) {
        base::Insert( root, key );
    }

    std::pair<ant::Index, bool> Index(T key) {
        try {
            return {base::Index( root, key ), true};
        } catch (std::runtime_error& error) {
            return {0, false};
        }
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(T key) {
        base::Remove( root, key );
    }

    bool empty() const {
        return !root;
    }

    ant::Count size() const {
        return BST<base::AVL_NodeIndexed<T>>::Size(root.get());
    }

    ConstIterator begin() const {
        if (root) return ConstIterator(root.get());
        return end();
    }

    ConstIterator end() const {
        return ConstIterator();
    }

    T Reduce(ant::Index pos, ant::Count count) {
        base::
    }
};

}