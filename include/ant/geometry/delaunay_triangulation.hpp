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

// need my paper with instructions



template<>
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
        
        bool IsInsideOrLie(Index v) const {
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
            } 
        private:
        
            ReplaceEdgeNode(Edge& e, Index from, Index to) {
                auto& ni = neighbors_[e];
                std::swap(ni[0] == from ? ni[0] : ni[1], to);
            }
            
            RemoveEdge(Edge& e) {
                neighbors_.erase(e);
            }
            
            InsertEdge(Edge& e, NodeIndex i_0, NodeIndex i_1) {
                neighbors_[e] = {i_0, i_1};
            }
        
            InsertEdge(Edge& e, NodeIndex i) {
            
            }
        
            // add arguments that are needed
            // need to delete old nodes
            void InsertOnEdge() {
                // could wrap everything inside loop of two iterations
                // but this implementation is more explicit
                auto ni = neighbors_[edge];
                RemoveEdge(edge);
                
                
                auto& n_0 = nodes_[ni[0]];
                auto trd_0 = n_0->trg.Third(edge);
                auto n_0_0 = new Node(Triangle(index, edge[0], trd_0));
                auto n_0_1 = new Node(Triangle(index, edge[1], trd_0));
                // new elements in neighbors
                i_0_0 = neighbors_.size();
                neighbors_.push_back(n_0_0);
                i_0_1 = neighbors_.size();
                neighbors_.push_back(n_0_1);
                ReplaceNode(ni[0], {{ i_0_0, i_0_1 }});
                
                ReplaceEdgeNode({edge[0], trd_0}, ni[0], i_0_0);
                ReplaceEdgeNode({edge[1], trd_0}, ni[0], i_0_1);
                
                InsertEdge({trd_0, index}, i_0_0, i_0_1);
                
    
                auto& n_1 = nodes_[ni[1]];
                auto trd_1 = n_1->trg.Third(edge);
                auto n_1_0 = new Node(Triangle(index, edge[0], trd_1));
                auto n_1_1 = new Node(Triangle(index, edge[1], trd_1));
                // new elements in neighbors
                i_1_0 = neighbors_.size();
                neighbors_.push_back(n_1_0);
                i_1_1 = neighbors_.size();
                neighbors_.push_back(n_1_1);
                ReplaceNode(ni[1], {{ i_1_0, i_1_1 }});
                
                ReplaceEdgeNode({edge[0], trd_1}, ni[1], i_1_0);
                ReplaceEdgeNode({edge[1], trd_1}, ni[1], i_1_1);
                
                InsertEdge({trd_1, index}, i_1_0, i_1_1);
                
                
                InsertEdge({edge[0], index}, i_0_0, i_1_0);
                InsertEdge({edge[1], index}, i_0_1, i_1_1);
            }
            
            // need triangle index
            void InsertInside(NodeIndex self) {
                // should probably make stuff explictily too
                for (Egde e : nodes_[self]->trg.Edges()) {
                    nodes_.push_back(new Node(Triangle(e, index)));
                    NodeIndex i_2 = nodes_.size()-1;
                    ReplaceEdgeNode(e, self, i_2);
                    InsertEdge({e[0], index}, i_2);
                    InsertEdge({e[1], index}, i_2);
                }
                auto last = nodes_.size()-1;
                ReplaceNode(self, {{last, last-1, last-2}});
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
                        break;
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
            auto& n_0 = nodes_[ni[0]];
            auto& n_1 = nodes_[ni[1]]; 
            
            RemoveEdge(edge);
            
            auto trd_0 = n_0->trg.Third(edge);
            auto trd_1 = n_1->trg.Third(edge);
            
            nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[0])));
            auto i_0 = nodes_.size()-1;
            nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[1])));
            auto i_1 = nodes_.size()-1;
            
            InsertEdge({trd_0, trd_1}, i_0, i_1);
            
            ReplaceEdgeNode({edge[0], trd_0}, ni[0], i_0);
            ReplaceEdgeNode({edge[1], trd_0}, ni[0], i_1);
            
            ReplaceEdgeNode({edge[0], trd_1}, ni[1], i_0);
            ReplaceEdgeNode({edge[1], trd_1}, ni[1], i_1);
            
            ReplaceNode(ni[0], {{ i_0, i_1 }});
            ReplaceNode(ni[1], {{ i_0, i_1 }});
        }
        
        template<Count N>
        ReplaceNode(Index i, std::array<NodeIndex, N>& children) {
            auto n = nodes_[i];
            nodes_[i] = new Node_n<N>(n->trg, children);
            delete n;
        }
        
        
        
        void Insert(const Index index) {
            root_->Insert(index);
        }
    };
    

    // first 3 points to ise
    // should i hide them somehow
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
