
#ifndef ANT_AVL_SET
#define ANT_AVL_SET


#include "ant/core/core.hpp"
#include "ant/core/bst_set.hpp"


namespace ant {



template<typename T, typename Compare = std::less<T>>
class AvlTree : public Bst<T> {

    struct Node : Bst<T>::Node {
        Count height;
    };
    
public:
        
    using iterator = typename Bst<T>::iterator;

    std::pair<iterator, bool> insert(const T& t) {
        // need to insert Node type inside
        auto p = Bst<T>::insert(t);
        Node* n = static_cast<Node*>(this->node(p.first));
        n->height = 1;
        Rebalance(n);
    }

    void Rebalance(Node* n) {
        
    }


};


}


#endif