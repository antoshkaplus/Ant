
#ifndef ANT_BST_SET_0
#define ANT_BST_SET_0


#include <iterator>


#include "ant/core/core.hpp"

// there are multiple possible implementations:
// 0 - use index for right and left node

// 1 - end can be inside root always... and we can move left everytime

// 2 - end can be most right vertex... and we need to watch her closely


// 4 - implementation without parent


namespace ant {

    
template<typename T, typename Compare = std::less<T>>
class BstSet_0 {
protected:

    static const Index LEFT = 0;
    static const Index RIGHT = 1;
    
    static constexpr Index OppositeDirection(Index dir) {
        return dir == LEFT ? RIGHT : LEFT;
    }
    
    class Node {
    public:
        Node* child_[2];
        Node* parent_  = nullptr; 
        T value_;
        
        Node() {}
        Node(const T& t) : value_(t) {}
        
        // prev and next are similar
        // just change left and right functions
        
        template<Index direction>
        Node* After() {
            constexpr Index opposite_direction = OppositeDirection(direction);
            auto n = this;
            if (Exists(Child(n, direction))) {
                n = Child(n, direction);
                while (Exists(Child(n, opposite_direction))) {
                    n = Child(n, opposite_direction);
                }
                return n;
            }
            auto n_2 = Parent(n);
            if (!Exists(n_2)) return nullptr;
            
            if (Child(n_2, opposite_direction) == n) {
                return n_2;
            } else {
                // n right child
                // will return nullptr if can't find anything
                while (Exists(n_2) && Child(n_2, opposite_direction) != n) {
                    n = n_2;
                    n_2 = Parent(n_2);
                }
                return n_2;
            }

        }
        
        Node* Prev() {
            return After<LEFT>();          
        }
        
        Node* Next() {
            return After<RIGHT>();
        }

        template<Index direction>
        Node* Last() {
            auto n = this;
            while (Exists(Child(n, direction))) {
                n = Child(n, direction);
            }
            return n;
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
        
        // deletes the whole subtree
        static void DeleteSubtree(Node* n) {
            if (!Exists(n)) return;
            // need to find out is right or left child then null everything
            if (Exists(Parent(n))) {
                if (Left(Parent(n)) == n) n->parent_->child_[LEFT] = nullptr;
                else n->parent_->child_[RIGHT] = nullptr;
                n->parent_ = nullptr;
            }
            // now just clear separate tree with n
            while (true) {
                if (Exists(Left(n))) {
                    n = Left(n);
                } else if (Exists(Right(n))) {
                    n = Right(n);
                } else {
                    auto n_old = n;
                    // if nullptr will finish
                    n = Parent(n);
                    delete n_old;
                    if (n == nullptr) break;
                    if (n_old == n->child_[RIGHT]) {
                        n->child_[RIGHT] = nullptr;
                    } else {
                        n->child_[LEFT] = nullptr;
                    }
                }
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
        
        friend class BstSet_0<T>;
    };

protected:

    std::default_random_engine rng_{(unsigned)std::chrono::system_clock::now().time_since_epoch().count()};
    size_t size_ = 0;
    // root equals end
    Node* end_ = new Node();
    Node* root_ = end_;
    Compare compare;
    
    
    Node* node(iterator it) {
        return it.current_;
    }
    
public:
   
    BstSet_0(const Compare& compare = Compare())
        : end_(new Node()), root_(end_), compare(compare) {}
    
    virtual ~BstSet_0() {
        clear();
        delete end_;
    }
    
    void clear() {
        Node::DeleteSubtree(root_);
        root_ = end_ = new Node();
        size_ = 0;
    }
    
    size_t size() const {
        return size_;
    }
        
    iterator begin() const {
        if (root_ == end_) return end();
        return iterator(root_->Min());
    } 
    
    // probably should just return nullptr as node
    iterator end() const {
        return iterator(end_);
    }
    
    iterator find(const T& t) const {
        auto n = root_; 
        while (n != nullptr && n != end_) {
            if (n->value_ < t) {
                n = n->child_[RIGHT];
                continue;
            }
            if (t < n->value_) {
                n = n->child_[LEFT];
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
        if (root_ == end_) {
            root_ = n_new;
            root_->child_[RIGHT] = end_;
            end_->parent_ = root_;
            size_ = 1;
            return {begin(), true};
        }
        auto n = root_;
        while (true) {
            if (n->value_ < t) {
                if (n->child_[RIGHT] == nullptr || n->child_[RIGHT] == end_) {
                    break;
                }
                n = n->child_[RIGHT];
                continue;
            }
            if (t < n->value_) {
                if (n->child_[LEFT] == nullptr) {
                    break;
                }
                n = n->child_[LEFT];
                continue;
            }
            return {iterator(n), false};
        }
        // now should have not null parent
        n_new->parent_ = n;
        if (t < n->value_) {
            n->child_[LEFT] = n_new;
        } else {
            // equality could not be
            if (n->child_[RIGHT] == end_) {
                n_new->child_[RIGHT] = end_;
                end_->parent_ = n_new;   
            }
            n->child_[RIGHT] = n_new;
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
        if (it == end()) return;
        --size_;
        auto n = it.current_;
        if (n->child_[RIGHT] == nullptr && n->child_[LEFT] == nullptr) {
            if (n->parent_ == nullptr) {
                root_ = nullptr;
            }
            else {
                n->parent_->ResetChild(n, nullptr);
            }
            delete n;
            return;
        }
        if (n->child_[RIGHT] == nullptr && n->child_[LEFT] != nullptr) {
            if (n->parent_ == nullptr) {
                root_ = n->child_[LEFT];
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->child_[LEFT]);
                n->child_[LEFT]->parent_ = n->parent_;
            }
            delete n;
            return;
        } 
        if (n->child_[LEFT] == nullptr && n->child_[RIGHT] != nullptr) {
            if (n->parent_ == nullptr) {
                root_ = n->child_[RIGHT];
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->child_[RIGHT]);
                n->child_[RIGHT]->parent_ = n->parent_;
            }
            delete n;
            return;
        }
      
        // both children present
        if (std::uniform_int_distribution<>(0, 1)(rng_) == 0 && n->child_[RIGHT] != end_) {
            // first left to right
            if (n->parent_ == nullptr) {
                root_ = n->child_[RIGHT];
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->child_[RIGHT]);
                n->child_[RIGHT]->parent_ = n->parent_;
            }
            auto p = n->child_[RIGHT]->Min();
            n->child_[LEFT]->parent_ = p;
            p->child_[LEFT] = n->child_[LEFT];
        } else { 
            // second right to left
            if (n->parent_ == nullptr) {
                root_ = n->child_[LEFT];
                root_->parent_ = nullptr;
            } else {
                n->parent_->ResetChild(n, n->child_[LEFT]);
                n->child_[LEFT]->parent_ = n->parent_;
            }
            auto p = n->child_[LEFT]->Max();
            n->child_[RIGHT]->parent_ = p;
            p->child_[RIGHT] = n->child_[RIGHT];
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