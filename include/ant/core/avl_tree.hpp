
#ifndef ANT_AVL_SET
#define ANT_AVL_SET


#include "ant/core/core.hpp"
#include "ant/core/bst.hpp"


namespace ant {


namespace avl {

template<class T>
struct Node;

template<class T>
using UN = std::unique_ptr<Node<T>>;

/* C-like structure representing an AVL tree node.
 * To have a std::set-like interface, see the avl class below.
 */
template<class T>
struct Node {
    T value;
    Count height;
    std::array<UN<T>, 2> children;
    
    Node() = default;
    Node(T value) : value(value) {}
    Node(T value, UN<T>&& left_child, UN<T>&& right_child) 
        : value(value) {
        
        children[0] = std::move(left_child);
        children[1] = std::move(right_child);
    }
    
};

constexpr std::array<int, 2> kAnother{{1, 0}};


/* Returns the height of the given node.
 * If the node is a null pointer, -1 is returned.
 */
template<class T>
int Height(const UN<T> & ptr) {
    return ptr ? ptr->h : -1;
}

/* Recompute and set the height attribute from the lchild and rchild nodes.
 */
template<class T>
void UpdateHeight(UN<T> & ptr) {
    ptr->h = std::max(Height(ptr->children[0]), Height(ptr->children[1])) + 1;
}

/* Assigns ptr2 to ptr1, ptr3 to ptr2, and ptr1 to ptr3,
 * without destroying any object.
 */
template<class T>
void CircularShiftUN( UN<T> & ptr_1, UN<T> & ptr_2, UN<T> & ptr_3 )
{
    ptr_1.swap(ptr_2);
    ptr_2.swap(ptr_3);
}

/* Performs a left and right rotation.
 * Node heights are adjusted accordingly.
 * ptr->children[rotation] is assumed to be non-null.
 */
template<class T>
void Rotate(UN<T>& ptr, int rotation) {
    CircularShiftUN(ptr, ptr->children[kAnother[rotation]], ptr->children[kAnother[rotation]]->children[rotation]);
    UpdateHeight(ptr->children[rotation]);
    UpdateHeight(ptr);
}


/* Make the tree rooted at ptr an AVL tree,
 * provided that both ptr->lchild and ptr->rchild are AVL trees
 * whose height differ by at most two.
 */
template<class T>
void FixAvl(UN<T>& ptr) {
    for (int i : {0, 1}) {
        if (Height(ptr->children[kAnother[i]]) < Height(ptr->children[i])) {
            if (Height(ptr->children[i]->children[kAnother[i]]) > Height(ptr->children[i]->children[i])) {
                Rotate(ptr->children[i], i);
                return;
            }
        }
    }
    UpdateHeight(ptr);
}

/* Inserts the given key in the given tree
 * and adjust it so that it continues to be an AVL tree.
 * tree->h is increased by at most one.
 */
template<class T>
void insert(UN<T>& tree, T value) {
    if( !tree) {
        tree = std::make_unique<UN<T>>(value);
    }
    else if(value < tree->value) {
        insert(tree->children[0], value);
    }
    else if(tree->value < value) {
        insert(tree->children[1], value);
    }
    else {
        // try to insert item that is already there
        // shouldn't we try to return something
        
        assert(false);
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
template<class T>
void RemoveMax(UN<T>& tree, UN<T>& ret ) {
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
template<class T>
void Remove(UN<T>& tree, const T& value) {
    if(!tree) return;
    if(value < tree->value) {
        Remove(tree->children[0], value);
    }
    else if(tree->value < value) {
        Remove(tree->children[1], value);
    }
    else {
        // Key is here.
        if(!tree->children[0]) {
            tree = std::move(tree->children[1]);
            return;
        }
        UN<T> tmp;
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
template<class T>
bool Contains(UN<T>& tree, const T& value ) {
    if( ! tree ) return false;
    if( value < tree->value )
        return contains( tree->children[0], value );
    if( tree->value < value )
        return contains( tree->children[1], value );
    return true;
}

// std::set-like interface
template<class T>
class AvlSet {
    UN<T> root;
public:
    // Returns 1 if the key was found in the tree, 0 otherwise.
    int count( int key ) {
        return Contains(root, key)? 1 : 0;
    }
    
    /* Inserts the key in the treap.
     * Nothing is done if the key is already there.
     */
    void Insert( int key ) {
        Insert( root, key );
    }
    
    /* Removes the given key from the treap.
     * Nothing is done if the key is not present.
     */
    void Erase( int key ) {
        Remove( root, key );
    }
};

// class AvlMap


}

}


#endif