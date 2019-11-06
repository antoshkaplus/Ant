#pragma once

#include "avl_tree_reduce.hpp"
#include "../bst_iterator_stack.hpp"

namespace ant::core::tree::avl {

template<class T_, class Func>
class AVL_SetReduce {
    using T = const T_;

    typename base::UN<base::AVL_NodeReduce<T>> root;
    Func op;
public:

    using ConstIterator = BST_IteratorStack<const base::AVL_NodeReduce<T>>;

    explicit AVL_SetReduce(Func&& func) : op(std::forward<Func>(func)) {}

    // Returns 1 if the key was found in the tree, 0 otherwise.
    int Count(T key) {
        return base::Contains(root, key)? 1 : 0;
    }

    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert(T key) {
        base::Insert( root, base::ParamsReduce(op), key );
    }

    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Remove(T key) {
        base::Remove( root, base::ParamsReduce(op), key );
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

    T Reduce(ant::Index pos, ant::Count count) const {
        return base::Reduce(root, op, pos, count);
    }
};

}