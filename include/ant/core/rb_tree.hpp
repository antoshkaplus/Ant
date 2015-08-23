
template<class T>
class RBTree {

    static const int L = 0;
    static const int R = 1;
    
    int Opposite(int d) {
        return d == R ? L : R;
    }
    
    using Dir = int;
    
    struct Node {
        Node(const T& data) : data(data) {
            is_red = true;
            child[0] = child[1] = nullptr;
        }

        bool is_red;
        T data;
        Node* child[2];
    };
    
    // if empty node then black
    bool IsRed(Node* n) {
        return n != NULL && n->is_red;
    }
    
    Node* SingleRotation(Node* root, Dir dir) {
        auto opp = Opposite(dir);
        auto save = root->child[opp];
        root->child[opp] = save->child[dir];
        save->child[dir] = root;
        
        root->is_red = true;
        save->is_red = false;
        
        return save;
    }
    
    Node* DoubleRotation(Node* root, Dir dir) {
        auto opp = Opposite(dir);
        root->child[opp] = SingleRotation(root->child[opp], opp);
        return SingleRotation(root, dir);
    }
    
    bool IsCorrect(Node* root) {
        try {
            CheckCorrectness(root);
        } catch (std::exception& e) {
            return false;
        }
        return true;
    }
    
    // returns tree height for recursive use 
    int CheckCorrectness(Node* root) {
        if (root = nullptr) {
            return 1;
        } else {
            auto ln = root->child[R];
            auto rn = root->child[L];
            
            // consecutive red links
            if (IsRed(root)) {
                if (IsRed(ln) || IsRed(rn)) {
                    throw std::runtime_error("Red violation");
                }
            }
            
            auto lh = CheckCorrectness(ln);
            auto rh = CheckCorrectness(rn);
            
            // invalid binary search tree
            if ((ln != nullptr && ln->data >= root->data) ||
                (rn != nullptr && rn->data <= root->data)) {
                
                throw std::runtime_error("Binary tree violation");
            }
                
            // black height mismatch
            if (lh != 0 && rh != 0 && lh != rh) {
                throw std::runtime_error("Black violation");
            }
            
            // only count black links
            // we know on which side to count
            return IsRed(root) ? lh : lh + 1; 
        }
    }
    
    // return bool that shows if value was inserted or not
    bool Insert(const T& data) {
        root_ = InsertSub(root_, data);
        root_->is_red = false;
    }
    
    bool InsertSub(Node* root, const T& data) {
        if (root == nullptr) {
            root = new Node(data);
        } else if (data != root->data) {
            // choose branch to go
            Dir dir = (root->data < data) ? L : R;
            root->child[dir] = InsertSub(root->child[dir], data);
            
            Dir opp = Opposite(dir);
            if (IsRed(root->child[dir])) {
                if (IsRed(root->child[opp])) {
                    root->is_red = true;
                    root->child[L]->is_red = root->child[R]->is_red = false;
                } else {
                    if (IsRed(root->child[dir]->child[dir])) {
                        root = SingleRotation(root, opp);
                    } else if (IsRed(root->child[dir]->child[opp])) {
                        root = DoubleRotation(root, opp);
                    }
                }
            }
        }
        return root;
    }
    
    
    Node* RemoveSub(Node* root, const T& data, bool& done) {
        if (root == nullptr) {
            done = true;
        } else {
            Dir dir;
            if (root->data == data) {
                if (root->child[L] == nullptr || root->child[R] == nullptr) {
                    Dir dd = root->child[L] == nullptr ? R : L;
                    Node* save = root->child[dd];
                    if (IsRed(root)) {
                        done = true;
                    } else if (IsRed(save)) {
                        save->is_red = false;
                        done = true;
                    }
                    
                    delete root;
                    return save;
                } else {
                    Node heir = root->child[L];
                    while (heir->child[R] != nullptr) {
                        heir = heir->child[R];
                    }
                    root->data = heir->data;
                    data = heir->data;
                }
            }
            
            dir = root->data < data;
            root->child[dir] = RemoveSub(root->child[dir], data, done);
            
            if (!done) {
                root = RemoveBalance(root, dir, done);
            }
        }
        return root;
    }
    
    bool Remove(const T& data) {
        bool done = false;
        root_ = RemoveSub(root_, data, done);
        if (root_ != nullptr) {
            root_->is_red = false;
        }
        return done;
    }
    
    Node* RemoveBalance(Node* root, Dir dir, bool done) {
        Dir opp = Opposite(dir);
        Node* p = root;
        Node* s = root->child[opp];
        
        if (s != nullptr && !IsRed(s)) {
            // black sibling cases
            if (!IsRed(s->child[L]) && !IsRed(s->child[R])) {
                if (IsRed(p)) {
                    done = true;
                }
                p->is_red = false;
                s->is_red = true;
            } else {
                bool save = root->is_red;
                if (IsRed(s->child[opp])) {
                    p = SingleRotation(p, dir);
                } else {
                    p = DoubleRotation(p, dir);
                }
                
                p->is_red = save;
                p->child[L]->is_red = false;
                p->child[R]->is_red = false;
                done = true;
            }
        } else if (s->child[dir] != nullptr) {
            Node* r = s->child[dir];
            if (!IsRed(r->child[L]) && !IsRed(r->child[R])) {
                p = SingleRotation(p, dir);
                p->child[dir]->child[opp]->is_red = true;
            } else {
                if (IsRed(r->child[dir])) {
                    s->child[dir] = SingleRotation(r, opp);
                }
                p = DoubleRotation(p, dir);
                s->child[dir]->is_red = false;
                p->child[opp]->is_red = true;
            }
            p->is_red = false;
            p->child[dir]->is_red = false;
            done = true;
        }
        return p;
    }
    
    
    Node* root_;

};
