
#ifndef ANT_BST_SET
#define ANT_BST_SET


#include <iterator>


#include "ant/core/core.hpp"



namespace ant {

    
template<typename T, typename Compare = std::less<T>>
class BstSet {
protected:

    class Node {
    public:
        
        Node() {}
        Node(const T& t) : value_(t) {}
        
        // prev and next are similar
        // just change left and right functions
        
        Node* Prev() {
            auto n = this;
            if (Exists(Left(n))) {
                n = Left(n);
                while (Exists(Right(n))) {
                    n = Right(n);
                }
                return n;
            }
            auto n_2 = Parent(n);
            if (!Exists(n_2)) return nullptr;
            
            if (Right(n_2) == n) {
                return n_2;
            } else {
                // n right child
                // will return nullptr if can't find anything
                while (Exists(n_2) && Right(n_2) != n) {
                    n = n_2;
                    n_2 = Parent(n_2);
                }
                return n_2;
            }
            
        }
        
        Node* Next() {
            auto n = this;
            if (Exists(Right(n))) {
                n = Right(n);
                while (Exists(Left(n))) {
                    n = Left(n);
                }
                return n;
            }
            auto n_2 = Parent(n);
            if (!Exists(n_2)) return nullptr;
            
            if (Left(n_2) == n) {
                return n_2;
            } else {
                // n left child
                // will return nullptr if can't find anything
                while (Exists(n_2) && Left(n_2) != n) {
                    n = n_2;
                    n_2 = Parent(n_2);
                }
                return n_2;
            }
        }

        
        // min element in subtree
        Node* Min() {
            auto n = this;
            while (Exists(Left(n))) {
                n = Left(n);
            }
            return n;
        }
        
        Node* Max() {
            auto n = this;
            while (Exists(Right(n))) {
                n = Right(n);
            }
            return n;
        }
        
        void ResetChild(Node* child, Node* new_child) {
            if (Left(this) == child) {
                left_ = new_child;
            } else {
                right_ = new_child;
            }
        }
        
        // deletes the whole subtree
        static void DeleteSubtree(Node* n) {
            if (!Exists(n)) return;
            // need to find out is right or left child then null everything
            if (Exists(Parent(n))) {
                if (Left(Parent(n)) == n) n->parent_->left_ = nullptr;
                else n->parent_->right_ = nullptr;
                n->parent_ = nullptr;
            }
            // now just clear separate tree with n
            while (n != nullptr) {
                if (Exists(Left(n))) {
                    n = Left(n);
                } else if (Exists(Right(n))) {
                    n = Right(n);
                } else {
                    auto n_old = n;
                    // if nullptr will finish
                    n = Parent(n);
                    delete n_old;
                }
            }
        }
        
        static Node* Left(Node* n) {
            return n->left_;
        }
        
        static Node* Right(Node* n) {
            return n->right_;
        }
        
        static Node* Parent(Node* n) {
            return n->parent_;
        }
        
        static bool Exists(Node* n) {
            return n != nullptr;
        } 
        
        Node* right_   = nullptr;
        Node* left_    = nullptr;
        Node* parent_  = nullptr; 
        T value_;
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
        
        // pred
        iterator& operator--() {
            // end
            current_ = current_->Prev(); 
            return *this;
        }
        // post
        iterator operator--(int) { 
            iterator it(current_);
            current_ = current_->Prev(); 
            return it;
        }
        
    private:
        Node* current_;
        
        friend class BstSet<T>;
    };

protected:

    std::default_random_engine rng_{(unsigned)std::chrono::system_clock::now().time_since_epoch().count()};
    size_t size_ = 0;
    // root equals end
    Node* root_ = nullptr;
    Node* end_ = new Node();
    Compare compare;
    
    
public:
   
    BstSet(const Compare& compare = Compare())
        : compare(compare) {}
    
    virtual ~BstSet() {
        clear();
        delete end_;
    }
    
    void clear() {
        Node::DeleteSubtree(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    size_t size() const {
        return size_;
    }
        
    iterator begin() const {
        if (root_ == nullptr) return end();
        return iterator(root_->Min());
    } 
    
    // probably should just return nullptr as node
    iterator end() const {
        end_->parent_ = root_ == nullptr ? nullptr : root_->Max();
        return iterator(end_);
    }
    
    iterator find(const T& t) const {
        auto n = root_; 
        while (n != nullptr) {
            if (n->value_ < t) {
                n = n->right_;
                continue;
            }
            if (t < n->value_) {
                n = n->left_;
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
        if (root_ == nullptr) {
            root_ = n_new;
            size_ = 1;
            return {begin(), true};
        }
        auto n = root_;
        while (true) {
            if (n->value_ < t) {
                if (n->right_ == nullptr) {
                    break;
                }
                n = n->right_;
                continue;
            }
            if (t < n->value_) {
                if (n->left_ == nullptr) {
                    break;
                }
                n = n->left_;
                continue;
            }
            return {iterator(n), false};
        }
        // now should have not null parent
        n_new->parent_ = n;
        if (t < n->value_) {
            n->left_ = n_new;
        } else {
            // equality could not be
            n->right_ = n_new;
        }
        ++size_; 
        return {iterator(n_new), true};
    }
    
    // should return iterator on next element or end
    void erase(const T& t) {
        erase(find(t));
    }
    
    void erase(iterator it) {
        if (it == end()) return;
        --size_;
        auto n = it.current_;
        if (n->right_ == nullptr && n->left_ == nullptr) {
            if (n->parent_ == nullptr) {
                root_ = nullptr;
            }
            else {
                n->parent_->ResetChild(n, nullptr);
            }
            delete n;
            return;
        }
        if (n->right_ == nullptr && n->left_ != nullptr) {
            if (n->parent_ == nullptr) {
                root_ = n->left_;
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->left_);
                n->left_->parent_ = n->parent_;
            }
            delete n;
            return;
        } 
        if (n->left_ == nullptr && n->right_ != nullptr) {
            if (n->parent_ == nullptr) {
                root_ = n->right_;
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->right_);
                n->right_->parent_ = n->parent_;
            }
            delete n;
            return;
        }
      
        if (std::uniform_int_distribution<>(0, 1)(rng_) == 0) {
            // first left to right
            if (n->parent_ == nullptr) {
                root_ = n->right_;
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->right_);
                n->right_->parent_ = n->parent_;
            }
            auto p = n->right_->Min();
            n->left_->parent_ = p;
            p->left_ = n->left_;
        } else { 
            // second right to left
            if (n->parent_ == nullptr) {
                root_ = n->left_;
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->left_);
                n->left_->parent_ = n->parent_;
            }
            auto p = n->left_->Max();
            n->right_->parent_ = p;
            p->right_ = n->right_;
        }
        delete n;
    }
    
private:
    
    // can use in find and insert
    iterator find_closest(const T& t) {
        return end();
    }
    
};



}


#endif