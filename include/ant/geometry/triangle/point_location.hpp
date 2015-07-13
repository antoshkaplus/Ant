
#pragma once


#include "triangle.hpp"
#include "ant/core/core.hpp"

#include <unordered_map>

namespace ant {

namespace geometry {
    
namespace triangle {


// this data structure is used by delaunay triangulation
// it stores a bunch of trianlges in tree-like data structure
template<class IsInsideType, class IsOnSegmentType>
class PointLocation {
    
    // to distinguish between index of trianle vertex (Index)
    // and tree nodes indices (NodeIndex)
    using NodeIndex = Index;
    using NI_2 = std::array<NodeIndex, 2>;
    using NI_3 = std::array<NodeIndex, 3>;
    
    class Node;
    
    using Nodes = std::vector<Node*>;
    using PL = PointLocation<IsInsideType, IsOnSegmentType>;
    
    // Leaf
    class Node {
    public:
        Triangle trg;
        
        Node(const Triangle& trg) {
            this->trg = trg;
        }
        
        virtual void Insert(Index index, NodeIndex self, PL& pl) {
            Edge edge;
            bool is_on_edge;
            // first check inside
            // after that check on edges?
            
            tie(edge, is_on_edge) = trg.OnEdge(index, (*pl.is_on_segment_));
            if (is_on_edge) {
                // probably better to do some looping
                // same for n_1
                InsertOnEdge(index, edge, pl);
            } else {
                InsertInside(index, self, pl);
            }   
        } 
        
        virtual void Print(std::ostream& output, Index self, const Nodes& ns) const {
            output << "node: " << self << std::endl;
            output << "triangle: " << trg << std::endl;
        }
        
        virtual ~Node() {}
         
    private:
        
        // add arguments that are needed
        // need to delete old nodes
        void InsertOnEdge(Index index, const Edge& edge, PL& pl) {
            // could wrap everything inside loop of two iterations
            // but this implementation is more explicit
            auto& neighbors = pl.neighbors_;
            auto& nodes = pl.nodes_;
            auto ni = neighbors[edge];
            pl.RemoveEdge(edge);
            
            
            auto& n_0 = nodes[ni[0]];
            auto trd_0 = n_0->trg.Third(edge);
            auto n_0_0 = new Node(Triangle(index, edge[0], trd_0));
            auto n_0_1 = new Node(Triangle(index, edge[1], trd_0));
            // new elements in neighbors
            NodeIndex i_0_0 = nodes.size();
            nodes.push_back(n_0_0);
            NodeIndex i_0_1 = nodes.size();
            nodes.push_back(n_0_1);
            pl.ReplaceNode<2>(ni[0], NI_2{{ i_0_0, i_0_1 }});
            
            pl.ReplaceEdgeNode({edge[0], trd_0}, ni[0], i_0_0);
            pl.ReplaceEdgeNode({edge[1], trd_0}, ni[0], i_0_1);
            
            pl.InsertEdge({trd_0, index}, i_0_0, i_0_1);
            
            auto& n_1 = nodes[ni[1]];
            auto trd_1 = n_1->trg.Third(edge);
            auto n_1_0 = new Node(Triangle(index, edge[0], trd_1));
            auto n_1_1 = new Node(Triangle(index, edge[1], trd_1));
            // new elements in neighbors
            NodeIndex i_1_0 = nodes.size();
            nodes.push_back(n_1_0);
            NodeIndex i_1_1 = nodes.size();
            nodes.push_back(n_1_1);
            pl.ReplaceNode<2>(ni[1], NI_2{{ i_1_0, i_1_1 }});
            
            pl.ReplaceEdgeNode({edge[0], trd_1}, ni[1], i_1_0);
            pl.ReplaceEdgeNode({edge[1], trd_1}, ni[1], i_1_1);
            
            pl.InsertEdge({trd_1, index}, i_1_0, i_1_1);
            
            
            pl.InsertEdge({edge[0], index}, i_0_0, i_1_0);
            pl.InsertEdge({edge[1], index}, i_0_1, i_1_1);
        }
        
        // need triangle index
        void InsertInside(Index index, NodeIndex self, PL& pl) {
            // should probably make stuff explictily too
            auto& nodes = pl.nodes_;
            for (Edge e : nodes[self]->trg.Edges()) {
                nodes.push_back(new Node(Triangle(e, index)));
                NodeIndex i_2 = nodes.size()-1;
                pl.ReplaceEdgeNode(e, self, i_2);
                pl.InsertEdge({e[0], index}, i_2);
                pl.InsertEdge({e[1], index}, i_2);
            }
            NodeIndex last = nodes.size()-1;
            pl.ReplaceNode<3>(self, NI_3{{last, last-1, last-2}});
        }
    
    };
    
    
    // can actually do iterating while instance is not usual Node
    
    template<Count N>
    class Node_n : public Node {
    public:
        std::array<NodeIndex, N> children;
        
        Node_n(Triangle& trg, const std::array<NodeIndex, N>& ns) : Node(trg) {
            children = ns;
        }
        
        virtual void Insert(Index index, NodeIndex self, PL& pl) override {
            for (auto i : children) {
                if (pl.IsInside(index, pl.nodes_[i]->trg)) {
                    pl.nodes_[i]->Insert(index, i, pl);
                    break;
                }
            }
            // now we check all edges
        }   
        
        virtual void Print(std::ostream& output, Index self, const Nodes& ns) const override {
            output << "node n: " << self << std::endl;
            output << Node::trg << std::endl;
            output << "children: ";
            ant::Print<NodeIndex, N>(output, children);
            for (auto ch : children) {
                ns[ch]->Print(output, ch, ns);
            }
        } 
        
        
    };
    
    using Node_2 = Node_n<2>;
    using Node_3 = Node_n<3>;
    
    
    static constexpr Index ROOT = 0;
    std::unordered_map<Edge, NI_2> neighbors_;
    // sometimes we would need changes of pointer to be seen for multiple parents
    // that's why we are going to use this shit
    // easy to expand easy to refer 
    std::vector<Node*> nodes_;
    const IsInsideType* is_inside_;
    const IsOnSegmentType* is_on_segment_; 
    
    // those are different operations we use on PointLocation members to 
    // relax Node and Node_n load 
    void ReplaceEdgeNode(const Edge& e, Index from, Index to) {
        auto& ni = neighbors_[e];
        std::swap(ni[0] == from ? ni[0] : ni[1], to);
    }
    
    void RemoveEdge(const Edge& e) {
        neighbors_.erase(e);
    }
    
    void InsertEdge(const Edge& e, NodeIndex i_0, NodeIndex i_1) {
        neighbors_[e] = {{i_0, i_1}};
    }
    
    void InsertEdge(const Edge& e, NodeIndex i) {
        auto it = neighbors_.find(e);
        if (it == neighbors_.end()) {
            neighbors_[e] = {{ i, -1 }};
        } else {
            auto& n = it->second; 
            char ch = (n[0] == -1) ? 0 : 1;
            n[ch] = i;
        }
    }
    
    bool IsInside(Index index, const Triangle& tr) const {
        return (*is_inside_)(index, tr);
    }
            
    template<Count N>
    void ReplaceNode(Index i, const std::array<NodeIndex, N>& children) {
        auto n = nodes_[i];
        nodes_[i] = new Node_n<N>(n->trg, children);
        delete n;
    }
    
public:
    
    
    PointLocation(const Triangle& trg, Count point_count, const IsInsideType& is_inside, const IsOnSegmentType& is_on_segment) {
        is_inside_ = &is_inside;
        is_on_segment_ = &is_on_segment;
        nodes_.reserve(3*point_count);
        nodes_.push_back(new Node(trg));
        for (auto e : trg.Edges()) {
            auto& v = neighbors_[e];
            v[0] = -1;
            v[1] = ROOT;
        }
    }
    
    void Flip(const Edge& edge) {
        // should remove this one afterwards
        // need to insert more guys afterwards
        NI_2 ni = neighbors_[edge];
        auto& n_0 = nodes_[ni[0]];
        auto& n_1 = nodes_[ni[1]]; 
        
        RemoveEdge(edge);
        
        auto trd_0 = n_0->trg.Third(edge);
        auto trd_1 = n_1->trg.Third(edge);
        
        nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[0])));
        Index i_0 = nodes_.size()-1;
        nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[1])));
        Index i_1 = nodes_.size()-1;
        
        InsertEdge({trd_0, trd_1}, i_0, i_1);
        
        ReplaceEdgeNode({edge[0], trd_0}, ni[0], i_0);
        ReplaceEdgeNode({edge[1], trd_0}, ni[0], i_1);
        
        ReplaceEdgeNode({edge[0], trd_1}, ni[1], i_0);
        ReplaceEdgeNode({edge[1], trd_1}, ni[1], i_1);
        
        ReplaceNode<2>(ni[0], {{ i_0, i_1 }});
        ReplaceNode<2>(ni[1], {{ i_0, i_1 }});
    }
    
    void Insert(const Index index) {
        nodes_[ROOT]->Insert(index, ROOT, *this);
    }
    
    void Print(ostream& output) const {
        output << "Printing PointLocation object" << std::endl;
        output << "Node tree contains " << nodes_.size() << " elements" << std::endl;
        nodes_[ROOT]->Print(output, ROOT, nodes_);
    }
    
    void PrintNodes(ostream& output) const {
        for (Index i = 0; i < nodes_.size(); ++i) {
            output << "node: " << i << std::endl;
            output << nodes_[i]->trg;
        }
    }
    
    void PrintNeighbors(ostream& output) const {
        for (auto p : neighbors_) {
            output << "edge: " << p.first[0] << " " << p.first[1] << ", nodes: " << p.second[0] << " " << p.second[1] << "\n";
        }
    }
    
    std::vector<Triangle> Triangles() const {
        return std::vector<Triangle>();
    }
    
    friend class Node;
    template<Count N> friend class Node_n; 
};


}

}

}

