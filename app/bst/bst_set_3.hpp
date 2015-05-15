
#pragma once


#include <iterator>


#include "ant/core/core.hpp"


namespace ant {


// won't use parent
// children array
// end is inside root
template<typename T, typename Compare = std::less<T>>
class BstSet_3 {
protected:
    
    static const Index LEFT = 0;
    static const Index RIGHT = 1;
    
    static constexpr Index OppositeDirection(Index dir) {
        return dir == LEFT ? RIGHT : LEFT;
    }
    
    class Node {
    public:
        Node* child_[2];
        T value_;
        
        Node() {}
        Node(const T& t) : value_(t) {}
        
        template<Index direction>
        Node* Last() {
            auto n = this;
            while (Exists(Child(n, direction))) {
                n = Child(n, direction);
            }
            return n;
        }
        
        bool IsLeft(Node* ch) {
            return child_[LEFT] == ch;
        }
        
        bool IsRight(Node* ch) {
            return child_[RIGHT] == ch;
        }
        
        Node* left() const {
            return child_[LEFT];
        }
        
        void set_left(Node* n) {
            child_[LEFT] = n;
        }
        
        Node* right() const {
            return child_[RIGHT];
        }
        
        void set_right(Node* n) {
            child_[RIGHT] = n;
        }
        
        // min element in subtree
        Node* Min() {
            return Last<LEFT>();
        }
        
        Node* Max() {
            return Last<RIGHT>();
        }
        
        void ResetChild(Node* child, Node* new_child) {
            if (Left(this) == child) {
                child_[LEFT] = new_child;
            } else {
                child_[RIGHT] = new_child;
            }
        }
        
        static Node* Child(Node* n, Index ch) {
            return n->child_[ch];
        }
        
        static Node* Left(Node* n) {
            return n->child_[LEFT];
        }
        
        static Node* Right(Node* n) {
            return n->child_[RIGHT];
        }
        
        static Node* Parent(Node* n) {
            return n->parent_;
        }
        
        static bool Exists(Node* n) {
            return n != nullptr;
        } 
    }; 
    

public:

    struct iterator : std::iterator<std::bidirectional_iterator_tag, T> {
        
        iterator(Node* node) : current_(node) {}
        iterator() {}
        
        const T& operator*() const { 
            return current_->value_;  
        } 
        
        bool operator==(const iterator& it) {
            return current_ == it.current_; 
        }
        bool operator!=(const iterator& it) {
            return current_ != it.current_;
        }
        
        // pred
        iterator& operator++() {
            current_ = current_->Next(); 
            return *this;
        }
        // post
        iterator operator++(int) { 
            iterator it(current_);
            current_ = current_->Next(); 
            return it;
        }
        
        Node* Next(Node* prev) {
            if (state_.empty()) return nullptr;
            auto t = state_.back();
            if (t->IsLeft(prev)) {
                // need to return t first
                if (prev->right() != nullptr) {
                    state_.push_back(prev);
                    prev = prev->right();
                    while (prev->left() != nullptr) {
                        prev = prev->left();
                        state_.push_back(prev);
                    }
                }
            }
            // we can't come from the right
            // should work fine.
            
            // similar shit for reverse iterator
        }
        
        
    private:
        std::vector<Node*> state_;
        
        friend class BstSet_3<T>;
    };

protected:

    std::default_random_engine rng_{(unsigned)std::chrono::system_clock::now().time_since_epoch().count()};
    size_t size_ = 0;
    // root equals end
    Node* root_ = new Node();
    Compare compare;
    
    
    Node* node(iterator it) {
        return it.current_;
    }
    
public:
   
    BstSet_3(const Compare& compare = Compare())
        : compare(compare) {}
    
    virtual ~BstSet_3() {
        //clear();
    }
    
    
    size_t size() const {
        return size_;
    }
        
    iterator begin() const {
        return iterator(root_->Min());
    } 
    
    // probably should just return nullptr as node
    iterator end() const {
        return iterator(root_);
    }
    
    iterator find(const T& t) const {
        auto n = root_->left(); 
        while (n != nullptr) {
            if (n->value_ < t) {
                n = n->right();
                continue;
            }
            if (t < n->value_) {
                n = n->left();
                continue;
            }
            return iterator(n);
        }
        return end();
    }
    
    Count count(const T& t) const {
        return exists(t) ? 1 : 0;
    }
    
    bool exists(const T& t) const {
        return find(t) != end();
    }
    
    // like find but should keep track of element parent
    std::pair<iterator, bool> insert(const T& t) {
        auto n_new = new Node(t); 
        if (root_->left() == nullptr) {
            root_->set_left(n_new);
            size_ = 1;
            return {begin(), true};
        }
        auto n = root_->left();
        while (true) {
            if (n->value_ < t) {
                if (n->right() == nullptr) {
                    break;
                }
                n = n->right();
                continue;
            }
            if (t < n->value_) {
                if (n->left() == nullptr) {
                    break;
                }
                n = n->left();
                continue;
            }
            return {iterator(n), false};
        }
        // now should have not null parent
        if (t < n->value_) {
            n->set_left(n_new);
        } else {
            // equality could not be
            n->set_right(n_new);
        }
        ++size_; 
        return {iterator(n_new), true};
    }
    
    template<typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }
    
            
    // should return iterator on next element or end
    void erase(const T& t) {
        erase(find(t));
    }
    
    void erase(iterator it) {
        // a lot of shit
    }
    
private:
    
    // can use in find and insert
    iterator find_closest(const T& t) {
        return end();
    }
    
};



}
