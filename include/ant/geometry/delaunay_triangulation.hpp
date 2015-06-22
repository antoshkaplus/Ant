//
//  delaunay_triangulation.hpp
//  ANT
//
//  Created by Anton Logunov on 6/18/15.
//
//

#ifndef ANT_delaunay_triangulation_hpp
#define ANT_delaunay_triangulation_hpp

/// probably we are going to work with integers here
// to prevent from (is point on the line debate)






class DelaunayTriangulation {

    using V_3 = std::array<Index, 3>;
    using V_2 = std::array<Index, 2>;
    
    struct Edge {
        V_2 vs;
        
        Edge(Index v_0, Index v_1) {
            vs[0] = v_0;
            vs[1] = v_1;
            std::sort(vs.begin(), vs.end());
        }
        
        Index operator[](Index i) const {
            return vs[i];
        }
        
    };

    struct Triangle {
        V_3 vs;
        
        Triangle(Edge& edge, Index v) {
            vs[0] = edge[0];
            vs[1] = edge[1];
            vs[2] = v;
            std::sort(vs.begin(), vs.end());
        }
        
        Index Third(Edge& e) const {
            if (e[0] != vs[0]) {
                return vs[0];
            } else if (e[1] != vs[1]) {
                return vs[1];
            } else return vs[2];
        }
        
        const std::array<Edge, 3> Edges() const {
            return {{ {vs[0],vs[1]}, {vs[1],vs[2]}, {vs[0],vs[2]} }};
        }
        
        std::pair<Edge, bool> OnEdge(Index v) const {
            // lol
        }
    };    
    
    template<class IsInside>
    class PointLocation {
        
        using NodeIndex = Index;
        using NI_2 = std::array<NodeIndex, 2>;
        using NI_3 = std::array<NodeIndex, 3>;
        
        
        // Leaf
        struct Node {
            Triangle trg;
            
            Node(Triangle& trg) {
                this->trg = trg;
            }
            
            // would need to put it as implementation
            virtual void Insert(Index index, NodeIndex self, std::vector<Node*>& nodes) {
                Edge edge;
                bool is_on_edge;
                tie(edge, is_on_edge) = trg.OnEdge(index);
                if (is_on_edge) {
                    // probably better to do some looping
                                       // same for n_1
                    InsertOnEdge();
                } else {
                    InsertInside();
                }   
                delete this;
            } 
        private:
        
            ReplaceNode(Edge& e, Index from, Index to) {
                auto& ni = neighbors_[e];
                std::swap(ni[0] == from ? ni[0] : ni[1], to);
            }
        
        
            void InsertOnEdge() {
                // this edge is no longer exists
                
                auto& n_0 = nodes_[ni[0]];
                auto& n_1 = nodes_[ni[1]];
                   
                
                
                
                
                auto ni_it = neighbors_.find(edge);
                auto ni = ni_it->second;
                neighbors_.erase(ni_it);
                
                for (auto I : ni) {
                    auto& n = nodes_[ni[I]];
                    auto& t = n->trg;
                    for (Edge e : t.Edges()) {
                        // need to delete - replace
                        // with one of triagles that i'm making
                        ni_it = neighbors_.find(e);
                        auto ni = ni_it->second;
                    } 
                    auto trd = t.Third(edge); 
                    for (auto J = 0; J < 2; ++J) {
                        nodes_.push_back(new Node(Triangle(index, edge[J], trd)));
                    }
                    auto last = nodes_.size() - 1;
                    n = new Node_2(t, {{ last, last-1 }});
                }
                
            }
            
            void InsertInside() {
                // just divide by three shit triangles
                for (Egde e : trg.Edges()) {
                    nodes_.push_back(new Node(Triangle(edge, index)));
                }
                auto last = nodes_.size()-1;
                nodes_[self] = new Node_3(trg, {{last, last-1, last-2}});
            }

        };
        
        // can actually do iterating while instance is not usual Node
        
        template<Count N>
        struct Node_n : Node {
            std::array<NodeIndex, N> children;
            
            Node_n(Triangle& trg, const std::array<NodeIndex, N>& ns) : Node(trg) {
                children = ns;
            }
            
            virtual void Insert(Index index, NodeIndex self, std::vector<Node*>& nodes) override {
                for (auto i : children) {
                    if (IsInside(nodes_[i]->trg)) {
                        nodes_[i]->Insert(index, i, nodes);
                    }
                }
            }        
        };
        
        using Node_2 = Node_n<2>;
        using Node_3 = Node_n<3>;
        
        
        std::unordered_map<Edge, NI_2> neighbors_;
        // sometimes we would need changes of pointer to be seen for multiple parents
        // that's why we are going to use this shit
        // easy to expand easy to refer 
        std::vector<Node*> nodes_;
        constexpr Index ROOT = 0;
        
        
    public:
    
        
        PointLocation(const Triangle& trg, Count point_count) {
            nodes_.reserve(3*point_count);
            nodes_.push_back(new Node(trg));
        }
        
        void Flip(Edge& egde) {
            // should remove this one afterwards
            // need to insert more guys afterwards
            auto NI_2 ni = neighbors_[edge];
            Index t_0 = nodes_[ni[0]].Trird(edge);
            Index t_1 = nodes_[ni[1]].Third(edge);
            
            nodes_.push_back(new Node(t_0, t_1, edge[0]));
            nodes_.push_back(new Node(t_0, t_1, edge[1]));
            
            NI_2 ni = {{nodes_.size()-2, nodes_.size()-1}};
            auto n_0 = nodes_[ni[0]];
            auto n_1 = nodes_[ni[1]];
            nodes_[ni[0]] = new Node_2(n_0.trg, ni);
            nodes_[ni[1]] = new Node_2(n_1.trg, ni);
            delete n_0;
            delete n_1;
        }
        
        
        void Insert(const Index index) {
            root_->Insert(index);
        }
    };
    

    // first 3 points to ise
    void Compute(vector<Point> ps, Point p_0, p_1, p_2) {
        
        for (auto& p : ps) {
            Triangle& tr = Find(p); 
            if (tr.IsInside(p)) {
                // divide triangle on three pieces
                LegalizeEdge() // inside new triangle
                
            } else {
                // if lies on an edge...
                // ... we do a lot of stuff
            }
            
        }
        // discarding p_1, p_2 with incident edges and p_0 too...

    }

    Triangle& Find(const Point& p) {
        
    }

    void LegalizeEdge() {
        if (illegal) {
            finding right triangle flip something
            and do legalize step again
            
        }
    }

    

};



#endif
