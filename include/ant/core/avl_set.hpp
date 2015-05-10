
#ifndef ANT_AVL_SET
#define ANT_AVL_SET

#include <iterator>


#include "ant/core/core.hpp"
#include "ant/core/bst_set.hpp"


namespace ant {



template<typename T, typename Compare = std::less<T>>
class AvlSet : BstSet<T> {

    struct Node : BstSet<T>::Node {
        Count height;
    };
    
    template<class V>
    class BaseIterator : std::iterator<std::bidirectional_iterator_tag, Node> {
    
        Node* node_;
        friend struct circular_list;
    
    public:    
        BaseIterator(Node* n) : node_(n) {} 
        
        V& operator*() const { return node_->value; }
        V* operator->() const { return node_->value; }
        
        bool operator==(const BaseIterator& it) const {
            return node_ == it.node_;
        }
        bool operator!=(const BaseIterator& it) const {
            return node_ != it.node_;
        }
        
        // those are a little bit different
        BaseIterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        // post iterator
        BaseIterator operator++(int) { 
            BaseIterator it(node_); 
            node_ = node_->next; 
            return it;
        }
        
        BaseIterator operator--() {
            node_ = node_->prev;
            return *this;
        }
        BaseIterator operator--(int) {
            BaseIterator it(node_);
            node_ = node_->prev;
            return it;
        }
    };
    
    
    Node* root;

public:    
    using ConstIterator = BaseIterator<const T>;

    ConstIterator begin() const {
        return ConstIterator();
    } 
    
    ConstIterator end() const {
        return ConstIterator();
    }


    bool Exists(const T& t) {
        if (root == nullptr) return false;
        
    }
    
    void Insert(const T& t) {
    
    }
    
    void Erase(const T& t) {
    
    }
    
    
    // also need iterator


};


}


#endif