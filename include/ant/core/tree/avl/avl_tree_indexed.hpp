#pragma once

#include "ant/core/core.hpp"
#include "ant/core/tree/bst.hpp"
#include "ant/core/tree/bst_iterator.hpp"


namespace ant::core::tree {

template <typename T>
struct AVL_NodeIndexed {
    using ValueType = T;

    T value_;
    Count size = 0;
    Count height;
    std::array<std::unique_ptr<AVL_NodeIndexed>, 2> children;

    AVL_NodeIndexed() = default;
    AVL_NodeIndexed(T value) : value_(value), size(1) {}
    AVL_NodeIndexed(T value, std::unique_ptr<AVL_NodeIndexed>&& left_child, std::unique_ptr<AVL_NodeIndexed>&& right_child)
            : value_(value) {

        children[0] = std::move(left_child);
        children[1] = std::move(right_child);
    }

    T& value() { return value_; }
    const T& value() const { return value_; }
};

template <class T>
struct AVL_BaseIndexed {

    constexpr static std::array<int, 2> kAnother{{1, 0}};

    using Node = AVL_NodeIndexed<T>;
    using UN = std::unique_ptr<Node>;

    /* Returns the height of the given node.
     * If the node is a null pointer, -1 is returned.
     */
    static int Height(const UN& ptr) {
        return ptr ? ptr->height : -1;
    }

    static int Size(const UN& ptr) {
        return ptr ? ptr->size : 0;
    }

    static int Size(const Node* ptr) {
        return ptr != nullptr ? ptr->size : 0;
    }

    /* Recompute and set the height attribute from the lchild and rchild nodes.
     */
    static void UpdateHeightSize(UN& ptr) {
        ptr->height = std::max(Height(ptr->children[0]), Height(ptr->children[1])) + 1;
        ptr->size = Size(ptr->children[0]) + Size(ptr->children[1]) + 1;
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
    static void Rotate(UN& ptr, int rotation) {
        CircularShiftUN(ptr, ptr->children[kAnother[rotation]], ptr->children[kAnother[rotation]]->children[rotation]);
        UpdateHeightSize(ptr->children[rotation]);
        UpdateHeightSize(ptr);
    }


    /* Make the tree rooted at ptr an AVL tree,
     * provided that both ptr->lchild and ptr->rchild are AVL trees
     * whose height differ by at most two.
     */
    static void FixAvl(UN& ptr) {
        for (int i : {0, 1}) {
            if (Height(ptr->children[kAnother[i]]) < Height(ptr->children[i])) {
                if (Height(ptr->children[i]->children[kAnother[i]]) > Height(ptr->children[i]->children[i])) {
                    Rotate(ptr->children[i], i);
                    return;
                }
            }
        }
        UpdateHeightSize(ptr);
    }

    /* Inserts the given key in the given tree
     * and adjust it so that it continues to be an AVL tree.
     * tree->h is increased by at most one.
     */
    static void InsertAt(UN& tree, Index index, T value) {
        if(!tree) {
            tree = std::make_unique<Node>(value);
        }
        else if(index < Size(Left(tree.get()))) {
            InsertAt(tree->children[0], index, value);
        }
        else if(index > Size(Left(tree.get()))) {
            index -= Size(Left(tree.get()))+1;
            InsertAt(tree->children[1], index, value);
        }
        else {
            std::swap(value, tree->value_);
            InsertAt(tree->children[1], 0, value);
        }
        FixAvl(tree);
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
    static void RemoveAt(UN& tree, Index index) {
        if(!tree) return;
        if(index < Size(Left(tree.get()))) {
            RemoveAt(tree->children[0], index);
        }
        else if(index > Size(Left(tree.get()))) {
            index -= Size(Left(tree.get()))+1;
            RemoveAt(tree->children[1], index);
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

    static T& At(UN& tree, Index index) {
        if(!tree) {
            throw std::runtime_error("out of range");
        }
        else if(index < Size(Left(tree.get()))) {
            return At(tree->children[0], index);
        }
        else if(index > Size(Left(tree.get()))) {
            index -= Size(Left(tree.get()))+1;
            return At(tree->children[1], index);
        }
        else {
            return tree->value_;;
        }
    }
};

}