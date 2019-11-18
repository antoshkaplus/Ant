#pragma once

#include "ant/core/core.hpp"
#include "ant/core/tree/bst.hpp"
#include "ant/core/tree/bst_iterator.hpp"


namespace ant::core::tree::avl::base {

template <typename T>
struct AVL_Node {
    using ValueType = T;

    T value_;
    Count height;
    std::array<std::unique_ptr<AVL_Node>, 2> children;

    AVL_Node() = default;
    AVL_Node(T value) : value_(value), height(1) {}
    AVL_Node(T value, std::unique_ptr<AVL_Node>&& left_child, std::unique_ptr<AVL_Node>&& right_child)
            : value_(value) {

        children[0] = std::move(left_child);
        children[1] = std::move(right_child);

        Update();
    }

    T& value() { return value_; }
    const T& value() const { return value_; }

    /* Recompute and set the height attribute from the lchild and rchild nodes.
     */
    void Update() {
        height = std::max(Height(children[0]), Height(children[1])) + 1;
    }
};

template <typename Node>
using UN = std::unique_ptr<Node>;

struct Params {
    constexpr static bool has_compare = false;

    template <typename Node>
    void update(UN<Node>& node) const {
        node->Update();
    }
};

template <typename Compare>
struct ParamsCompare {
    constexpr static bool has_compare = true;

    Compare& compare;

    explicit ParamsCompare(Compare&& compare) : compare(compare) {}

    template <typename Node>
    void update(UN<Node>& node) const {
        node->Update();
    }
};

/* Returns the height of the given node.
 * If the node is a null pointer, -1 is returned.
 */
template <typename Node>
int Height(const UN<Node>& ptr) {
    return ptr ? ptr->height : -1;
}

/* Assigns ptr2 to ptr1, ptr3 to ptr2, and ptr1 to ptr3,
 * without destroying any object.
 */
template <typename Node>
void CircularShiftUN(UN<Node>& ptr_1, UN<Node>& ptr_2, UN<Node>& ptr_3) {
    ptr_1.swap(ptr_2);
    ptr_2.swap(ptr_3);
}

/* Performs a left and right rotation.
 * Node heights are adjusted accordingly.
 * ptr->children[rotation] is assumed to be non-null.
 */
template <typename Node, typename Params>
void Rotate(UN<Node>& ptr, Params& params, int rotation) {
    CircularShiftUN(ptr, ptr->children[kAnother[rotation]], ptr->children[kAnother[rotation]]->children[rotation]);
    params.update(ptr->children[rotation]);
    params.update(ptr);
}


/* Make the tree rooted at ptr an AVL tree,
 * provided that both ptr->lchild and ptr->rchild are AVL trees
 * whose height differ by at most two.
 */
template <typename Node, typename Params>
void FixAvl(UN<Node>& ptr, Params& params) {
    for (int i : {0, 1}) {
        if (Height(ptr->children[kAnother[i]]) < Height(ptr->children[i]) - 1) {
            if (Height(ptr->children[i]->children[kAnother[i]]) > Height(ptr->children[i]->children[i])) {
                Rotate(ptr->children[i], params, i);
            }
            Rotate(ptr, params, kAnother[i]);
            return;
        }
    }
    params.update(ptr);
}

/* Inserts the given key in the given tree
 * and adjust it so that it continues to be an AVL tree.
 * tree->h is increased by at most one.
 */
template <typename Node, typename Params, typename std::enable_if<!std::remove_reference<Params>::type::has_compare>::type* = nullptr>
bool Insert(UN<Node>& tree, Params&& params, typename Node::ValueType value) {
    bool result = true;
    if(!tree) {
        tree = std::make_unique<Node>(value);
    }
    else if(value < tree->value_) {
        result = Insert(tree->children[0], params, value);
    }
    else if(tree->value_ < value) {
        result = Insert(tree->children[1], params, value);
    }
    else {
        // try to insert item that is already there
        // shouldn't we try to return something
        result = false;
    }
    FixAvl(tree, params);
    return result;
}

template <typename Node, typename Params, typename std::enable_if<std::remove_reference<Params>::type::has_compare>::type* = nullptr>
bool Insert(UN<Node>& tree, Params&& params, typename Node::ValueType value) {
    bool result = true;
    if(!tree) {
        tree = std::make_unique<Node>(value);
    }
    else if(params.compare(value, tree->value_)) {
        result = Insert(tree->children[0], params, value);
    }
    else if(params.compare(tree->value_, value)) {
        result = Insert(tree->children[1], params, value);
    }
    else {
        // try to insert item that is already there
        // shouldn't we try to return something
        result = false;
    }
    FixAvl(tree, params);
    return result;
}

/* Removes the maximum value of the given tree.
 * The node that contains the maximum value is stored in 'ret'.
 * The height of the tree is reduced at most by one.
 *
 * ret is passed by reference as we would go through recursive calls
 * current implementation should be faster. but we always can check ourselves
 */
template <typename Node, typename Params>
void RemoveMax(UN<Node>& tree, Params& params, UN<Node>& ret) {
    if(!tree->children[1] ) {
        // This is the maximum.
        ret = std::move(tree);
        tree = std::move(ret->children[0]);
    }
    else {
        RemoveMax(tree->children[1], params, ret);
        FixAvl(tree, params);
    }
}

/* Removes the given key from the tree.
 */
template <typename Node, typename Params>
void Remove(UN<Node>& tree, Params&& params, const typename Node::ValueType& value) {
    if(!tree) return;
    if(value < tree->value_) {
        Remove(tree->children[0], params, value);
    }
    else if(tree->value_ < value) {
        Remove(tree->children[1], params, value);
    }
    else {
        // Key is here.
        if(!tree->children[0]) {
            tree = std::move(tree->children[1]);
            return;
        }
        UN<Node> tmp;
        RemoveMax(tree->children[0], params, tmp);
        for (int i : {0, 1}) {
            tmp->children[i] = std::move(tree->children[i]);
        }
        tree = std::move(tmp);
    }
    FixAvl(tree, params);
}

template <typename Node, typename Params, typename Key,
        typename std::enable_if<!std::is_same<const Key, const typename Node::ValueType>::value>::type* = nullptr>
void Remove(UN<Node>& tree, Params&& params, const Key& key) {
    if(!tree) return;
    if(params.compare(key, tree->value_)) {
        Remove(tree->children[0], params, key);
    }
    else if(params.compare(tree->value_, key)) {
        Remove(tree->children[1], params, key);
    }
    else {
        // Key is here.
        if(!tree->children[0]) {
            tree = std::move(tree->children[1]);
            return;
        }
        UN<Node> tmp;
        RemoveMax(tree->children[0], params, tmp);
        for (int i : {0, 1}) {
            tmp->children[i] = std::move(tree->children[i]);
        }
        tree = std::move(tmp);
    }
    FixAvl(tree, params);
}

/* Decides whether the given tree has the specified key or not.
 */
template <typename Node>
bool Contains(UN<Node>& tree, const typename Node::ValueType& value ) {
    if( ! tree ) return false;
    if( value < tree->value_ )
        return Contains( tree->children[0], value );
    if(tree->value_ < value )
        return Contains( tree->children[1], value );
    return true;
}

template <typename Node, typename Key, typename Compare>
bool Contains(UN<Node>& tree, const Key& key, Compare comp) {
    if( ! tree ) return false;
    if( comp(key, tree->value_) )
        return Contains( tree->children[0], key, comp );
    if( comp(tree->value_, key) )
        return Contains( tree->children[1], key, comp );
    return true;
}


template <typename Node>
const Node* Left(const Node* n) {
    return n->children[0].get();
}

template <typename Node>
const Node* Right(const Node* n) {
    return n->children[1].get();
}

template <typename Node>
Node* Left(Node* n) {
    return n->children[0].get();
}

template <typename Node>
Node* Right(Node* n) {
    return n->children[1].get();
}

}