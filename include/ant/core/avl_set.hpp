
#ifndef ANT_AVL_SET
#define ANT_AVL_SET


#include "ant/core/core.hpp"
#include "ant/core/bst_set.hpp"


namespace ant {



template<typename T, typename Compare = std::less<T>>
class AvlSet : public BstSet<T> {

    struct Node : BstSet<T>::Node {
        Count height;
    };
    
public:
        
    using iterator = typename BstSet<T>::iterator;

    std::pair<iterator, bool> insert(const T& t) {
        // need to insert Node type inside
        auto p = BstSet<T>::insert(t);
        Node* n = static_cast<Node*>(this->node(p.first));
        n->height = 1;
        Rebalance(n);
    }

    void Rebalance(Node* n) {
        
    }


};


}


#endif