
#pragma once


#include <iterator>


#include "ant/core/core.hpp"


namespace ant {

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
    
    template<Index direction>
    struct base_iterator : std::iterator<std::forward_iterator_tag, T> {
        
        static constexpr Index dir = direction;
        static constexpr Index opp_dir = OppositeDirection(dir);  
        
        base_iterator() {
            state_.push_back(nullptr);
        }
        
        base_iterator(Node* root) {
            Init(root);
        }
        
        // nullptr first item looks for end
        // could use some empty node from tree itself
        void Init(Node* root) {
            state_.push_back(nullptr);
            Node* node = root;
            while (node != nullptr) {
                state_.push_back(node);
                node = node->child_[opp_dir];
            }
        }
    
        void After() {
            Node* node = state_.back();
            state_.pop_back();
            if (node->child_[dir] != nullptr) { 
                if (node->child_[dir] != nullptr) {
                    node = node->child_[dir];
                    state_.push_back(node);
                    while (node->child_[opp_dir] != nullptr) {
                        node = node->child_[opp_dir];
                        state_.push_back(node);
                    }
                }
            }
        }
        
        const T& operator*() const { 
            return state_.back()->value_;  
        } 
        
        bool operator==(const base_iterator& it) {
            return state_.back() == it.state_.back(); 
        }
        bool operator!=(const base_iterator& it) {
            return state_.back() != it.state_.back();
        }
        
        // pred
        base_iterator& operator++() {
            After(); 
            return *this;
        }
        
        // post
        base_iterator operator++(int) { 
            base_iterator it(*this);
            After(); 
            return it;
        }        
    
    protected:
        std::vector<Node*> state_;
        
        friend class BstSet_3<T>;
    };
    
    struct remove_iterator : base_iterator<RIGHT> {
        
        void After() {
            Node* node = base_iterator<RIGHT>::state_.back();
            base_iterator<RIGHT>::After();
            delete node;
        }
    };


public:

    using iterator = base_iterator<RIGHT>;
    using reverse_iterator = base_iterator<LEFT>;

protected:

    std::default_random_engine rng_{(unsigned)std::chrono::system_clock::now().time_since_epoch().count()};
    size_t size_ = 0;
    // root equals end
    Node* root_ = nullptr;
    Compare compare;
    
    
    Node* node(iterator it) {
        return it.current_;
    }
    
public:
   
    BstSet_3(const Compare& compare = Compare())
        : compare(compare) {}
    
    virtual ~BstSet_3() {
        clear();
    }
    
    
    size_t size() const {
        return size_;
    }
        
    iterator begin() const {
        return iterator(root_);
    } 
    
    reverse_iterator rbegin() const {
        return reverse_iterator(root_);
    }
    
    // probably should just return nullptr as node
    iterator end() const {
        return iterator();
    }
    
    reverse_iterator rend() const {
        return reverse_iterator();
    }
    
    iterator find(const T& t) const {
        auto n = root_; 
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
        if (root_ == nullptr) {
            root_ = n_new;
            size_ = 1;
            return {begin(), true};
        }
        auto n = root_;
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
    
    void erase(const T& t) {
        Node *prev = nullptr;
        Node *n = root_;
        while (true) {
            if (n->value_ < t) {
                prev = n;
                n = n->right();
                continue;
            }
            if (t < n->value_) {
                prev = n;
                n = n->left();
                continue;
            }
            break;
        }
        if (n->right() == nullptr && n->left() == nullptr) {
            if (prev == nullptr) {
                root_ = nullptr;
            }
            else {
                prev->ResetChild(n, nullptr);
            }
            delete n;
            return;
        }
        if (n->right() == nullptr && n->left() != nullptr) {
            if (prev == nullptr) {
                root_ = n->left();
            } else {
                prev->ResetChild(n, n->left());
            }
            delete n;
            return;
        } 
        if (n->left() == nullptr && n->right() != nullptr) {
            if (prev == nullptr) {
                root_ = n->right();
            } else {
                prev->ResetChild(n, n->right());
            }
            delete n;
            return;
        }
        
        // both children present
        if (std::uniform_int_distribution<>(0, 1)(rng_) == 0) {
            // first left to right
            if (prev == nullptr) {
                root_ = n->right();
            } else {
                prev->ResetChild(n, n->right());
            }
            auto p = n->right()->Min();
            p->set_left(n->left());
        } else { 
            // second right to left
            if (prev == nullptr) {
                root_ = n->left();
            } else {
                prev->ResetChild(n, n->left());
            }
            auto p = n->left()->Max();
            p->set_right(n->right());
        }
        delete n;

    }
    
    void clear() {
        reverse_iterator it(root_);
        reverse_iterator end;
        while (it != end) {
            ++it;
        }
        root_ = nullptr;
    }
};



}
