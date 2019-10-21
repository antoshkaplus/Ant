#pragma once

#include "ant/core/core.hpp"
#include "ant/core/tree/bst.hpp"
#include "ant/core/tree/bst_iterator.hpp"


namespace ant::core::tree {

template <typename T>
struct AVL_NodeReduce {
    using ValueType = T;

    T value_;
    T opRes;
    Count size;
    Count height;
    std::array<std::unique_ptr<AVL_Node>, 2> children;

    AVL_Node() = default;
    AVL_Node(T value) : value_(value) {}
    AVL_Node(T value, std::unique_ptr<AVL_Node>&& left_child, std::unique_ptr<AVL_Node>&& right_child)
            : value_(value) {

        children[0] = std::move(left_child);
        children[1] = std::move(right_child);
    }

    T& value() { return value_; }
    const T& value() const { return value_; }
};

template <class T>
struct AVL_BaseReduce {

    constexpr static std::array<int, 2> kAnother{{1, 0}};

    using Node = AVL_NodeReduce<T>;
    using UN = std::unique_ptr<Node>;

    /* Returns the height of the given node.
     * If the node is a null pointer, -1 is returned.
     */
    static int Height(const UN& ptr) {
        return ptr ? ptr->height : -1;
    }

    /* Recompute and set the height attribute from the lchild and rchild nodes.
     */
    template <typename Op>
    static void UpdateHeightSizeReduce(UN& ptr, Op op) {
        ptr->height = std::max(Height(ptr->children[0]), Height(ptr->children[1])) + 1;
        ptr->size = Size(ptr->children[0]) + Size(ptr->children[1]) + 1;
        ptr->opRes = ptr->value;
        if (ptr->children[0]) ptr->opRes = op(ptr->opRes, ptr->children[0]->opRes);
        if (ptr->children[1]) ptr->opRes = op(ptr->opRes, ptr->children[1]->opRes);
    }

    /* Assigns ptr2 to ptr1, ptr3 to ptr2, and ptr1 to ptr3,
     * without destroying any object.
     */
    static void CircularShiftUN( UN & ptr_1, UN & ptr_2, UN & ptr_3 )
    {
        ptr_1.swap(ptr_2);
        ptr_2.swap(ptr_3);
    }

    /* Performs a left and right rotation.
     * Node heights are adjusted accordingly.
     * ptr->children[rotation] is assumed to be non-null.
     */
    template <typename Op>
    static void Rotate(UN& ptr, int rotation, Op op) {
        CircularShiftUN(ptr, ptr->children[kAnother[rotation]], ptr->children[kAnother[rotation]]->children[rotation]);
        UpdateHeightSizeReduce(ptr->children[rotation], op);
        UpdateHeightSizeReduce(ptr, op);
    }


    /* Make the tree rooted at ptr an AVL tree,
     * provided that both ptr->lchild and ptr->rchild are AVL trees
     * whose height differ by at most two.
     */
    template <typename Op>
    static void FixAvl(UN& ptr, Op op) {
        for (int i : {0, 1}) {
            if (Height(ptr->children[kAnother[i]]) < Height(ptr->children[i])) {
                if (Height(ptr->children[i]->children[kAnother[i]]) > Height(ptr->children[i]->children[i])) {
                    Rotate(ptr->children[i], i, op);
                    return;
                }
            }
        }
        UpdateHeightSizeReduce(ptr, op);
    }

    /* Inserts the given key in the given tree
     * and adjust it so that it continues to be an AVL tree.
     * tree->h is increased by at most one.
     */
    template <typename Op>
    static bool Insert(UN& tree, T value, Op op) {
        bool result = true;
        if(!tree) {
            tree = std::make_unique<Node>(value);
        }
        else if(value < tree->value_) {
            result = Insert(tree->children[0], value, op);
        }
        else if(tree->value_ < value) {
            result = Insert(tree->children[1], value, op);
        }
        else {
            // try to insert item that is already there
            // shouldn't we try to return something
            result = false;
        }
        FixAvl(tree, op);
        return result;
    }

    /* Removes the maximum value of the given tree.
     * The node that contains the maximum value is stored in 'ret'.
     * The height of the tree is reduced at most by one.
     *
     * ret is passed by reference as we would go through recursive calls
     * current implementation should be faster. but we always can check ourselves
     */
    static void RemoveMax(UN& tree, UN& ret ) {
        if(!tree->children[1] ) {
            // This is the maximum.
            ret = std::move(tree);
            tree = std::move(ret->children[0]);
        }
        else {
            RemoveMax(tree->children[1], ret);
            FixAvl(tree);
        }
    }

    /* Removes the given key from the tree.
     */
    static void Remove(UN& tree, const T& value) {
        if(!tree) return;
        if(value < tree->value_) {
            Remove(tree->children[0], value);
        }
        else if(tree->value_ < value) {
            Remove(tree->children[1], value);
        }
        else {
            // Key is here.
            if(!tree->children[0]) {
                tree = std::move(tree->children[1]);
                return;
            }
            UN tmp;
            RemoveMax(tree->children[0], tmp);
            for (int i : {0, 1}) {
                tmp->children[i] = std::move(tree->children[i]);
            }
            tree = std::move(tmp);
        }
        FixAvl(tree);
    }

    /* Decides whether the given tree has the specified key or not.
     */
    static bool Contains(UN& tree, const T& value ) {
        if( ! tree ) return false;
        if( value < tree->value_ )
            return Contains( tree->children[0], value );
        if(tree->value_ < value )
            return Contains( tree->children[1], value );
        return true;
    }
};

}