
#pragma once

#include <unordered_map>

#include "ant/core/core.hpp"
#include "triangle.hpp"
#include "adjacent_triangles.hpp"


namespace ant {
namespace geometry {
namespace triangle {

// ??? indeces ???
// inside each triangle you keep pointers on edges
// each edge has two indices on it's leaf triangles



// this data structure is used by delaunay triangulation
// it stores a bunch of trianlges in tree-like data structure

// using 2 args in base class and add 3-rd argument when we want delanauy triangulation

// takes two args (index, triangle) => but you want to keep circle with triangle... on creation 

// just need to be able to convert to circle
template<class IsInsideType, class IsOnSegmentType>
class PointLocation {
protected:    
    // to distinguish between index of trianle vertex (Index)
    // and tree nodes indices (NodeIndex)
    using NodeIndex = Index;
    using NI_2 = std::array<NodeIndex, 2>;
    using NI_3 = std::array<NodeIndex, 3>;

protected:
    class Node;
private:
    using Nodes = std::vector<Node*>;
    using PL = PointLocation<IsInsideType, IsOnSegmentType>;
    
protected:
    // need to be able to inherit this guy
    
    // Leaf
    class Node {
    public:
        Triangle trg;
        
        Node(const Triangle& trg) {
            this->trg = trg;
        }
        
        virtual ~Node() {}
        
        virtual bool IsLeaf() const {
            return true;
        }
        
        virtual void Insert(Index index, NodeIndex self, PL& pl) {
            Edge edge;
            bool is_on_edge;
            // first check inside
            // after that check on edges?
            
            std::tie(edge, is_on_edge) = trg.OnEdge(index, (*pl.is_on_segment_));
            if (is_on_edge) {
                // probably better to do some looping
                // same for n_1
                InsertOnEdge(index, edge, pl);
                pl.PostInsertOnEdge(index, edge);
            } else {
                InsertInside(index, self, pl);
                pl.PostInsertInside(index, trg);
            }   
        } 
        
        virtual void Print(std::ostream& output, Index self, const Nodes& ns) const {
            output << "node: " << self << std::endl;
            output << "triangle: " << trg << std::endl;
        }
            
    private:
        
        // add arguments that are needed
        // need to delete old nodes
        void InsertOnEdge(Index index, const Edge& edge, PL& pl) {
            // could wrap everything inside loop of two iterations
            // but this implementation is more explicit
            auto& neighbors = pl.neighbors_;
            auto& nodes = pl.nodes_;
            auto ni = neighbors[edge];
            neighbors.Remove(edge);
            
            
            auto& n_0 = nodes[ni[0]];
            auto trd_0 = n_0->trg.Third(edge);
            auto i_0_0 = pl.AddLeafNode(Triangle(index, edge[0], trd_0));
            auto i_0_1 = pl.AddLeafNode(Triangle(index, edge[1], trd_0));
            pl.ReplaceNode<2>(ni[0], NI_2{{ i_0_0, i_0_1 }});
            
            neighbors.Replace({edge[0], trd_0}, ni[0], i_0_0);
            neighbors.Replace({edge[1], trd_0}, ni[0], i_0_1);
            
            neighbors.Insert({trd_0, index}, i_0_0, i_0_1);
            
            auto& n_1 = nodes[ni[1]];
            auto trd_1 = n_1->trg.Third(edge);
            auto i_1_0 = pl.AddLeafNode(Triangle(index, edge[0], trd_1));
            auto i_1_1 = pl.AddLeafNode(Triangle(index, edge[1], trd_1));
            pl.ReplaceNode<2>(ni[1], NI_2{{ i_1_0, i_1_1 }});
            
            neighbors.Replace({edge[0], trd_1}, ni[1], i_1_0);
            neighbors.Replace({edge[1], trd_1}, ni[1], i_1_1);
            
            neighbors.Insert({trd_1, index}, i_1_0, i_1_1);
            
            
            neighbors.Insert({edge[0], index}, i_0_0, i_1_0);
            neighbors.Insert({edge[1], index}, i_0_1, i_1_1);
        }
        
        // need triangle index
        void InsertInside(Index index, NodeIndex self, PL& pl) {
            // should probably make stuff explictily too
            auto& neighbors = pl.neighbors_;
            auto& nodes = pl.nodes_;
            for (Edge e : nodes[self]->trg.Edges()) {
                NodeIndex i_2 = pl.AddLeafNode(Triangle(e, index));
                neighbors.Replace(e, self, i_2);
                neighbors.Insert({e[0], index}, i_2);
                neighbors.Insert({e[1], index}, i_2);
            }
            NodeIndex last = nodes.size()-1;
            pl.ReplaceNode<3>(self, NI_3{{last, last-1, last-2}});
        }
        
        
    };
    
private:
    // can actually do iterating while instance is not usual Node
    
    template<Count N>
    class Node_n : public Node {
    public:
        std::array<NodeIndex, N> children;
        
        Node_n(Triangle& trg, const std::array<NodeIndex, N>& ns) : Node(trg) {
            children = ns;
        }
        
        virtual ~Node_n() {}
        
        virtual bool IsLeaf() const override {
            return false;
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
            ant::Println<NodeIndex, N>(output, children);
            for (auto ch : children) {
                ns[ch]->Print(output, ch, ns);
            }
        } 
    };
    
    using Node_2 = Node_n<2>;
    using Node_3 = Node_n<3>;
    
    
    bool IsInside(Index index, const Triangle& tr) const {
        return (*is_inside_)(index, tr);
    }
            
    template<Count N>
    void ReplaceNode(Index i, const std::array<NodeIndex, N>& children) {
        auto n = nodes_[i];
        nodes_[i] = new Node_n<N>(n->trg, children);
        delete n;
    }

protected:

    const Nodes& nodes() const {
        return nodes_;
    }

    virtual void PostInsertInside(Index i, const Triangle& trg) {}
    
    virtual void PostInsertOnEdge(Index i, const Edge& e) {}
    
    NodeIndex AddLeafNode(const Triangle& trg) {
        nodes_.push_back(NewLeafNode(trg));
        return nodes_.size()-1;
    }
    
    // used only inside this class
    virtual Node* NewLeafNode(const Triangle& trg) {
        return new Node(trg);
    } 
    

public:
    
    
    PointLocation(Count point_count, const IsInsideType& is_inside, const IsOnSegmentType& is_on_segment) {
        is_inside_ = &is_inside;
        is_on_segment_ = &is_on_segment;
        nodes_.reserve(3*point_count);
    }
    
    // need to work on destructor
    ~PointLocation() {
        for (auto n : nodes_) {
            delete n;
        }
    }
    
    
    virtual void Init(const Triangle& trg) {
        AddLeafNode(trg);
        for (auto e : trg.Edges()) {
            neighbors_.Insert(e, ROOT, -1);
        }
    }
    
    // we don't use reference here because after we remove this edge
    // reference is going to be invalid
    void Flip(Edge edge) {
        // should remove this one afterwards
        // need to insert more guys afterwards
        
        NI_2 ni = neighbors_[edge];
        auto& n_0 = nodes_[ni[0]];
        auto& n_1 = nodes_[ni[1]]; 
        
        assert(n_0->IsLeaf() && n_1->IsLeaf());
        
        // here we should do replace instead of remove => insert
        neighbors_.Remove(edge);
        
        auto trd_0 = n_0->trg.Third(edge);
        auto trd_1 = n_1->trg.Third(edge);
        
        //std::cout << edge[0] << " " << edge[1] << " " << trd_0 << " " << trd_1 << std::endl;
        assert(trd_0 != trd_1);
        
        NodeIndex i_0 = AddLeafNode(Triangle(trd_0, trd_1, edge[0]));
        NodeIndex i_1 = AddLeafNode(Triangle(trd_0, trd_1, edge[1]));
        neighbors_.Insert({trd_0, trd_1}, i_0, i_1);
        
        neighbors_.Replace({edge[0], trd_0}, ni[0], i_0);
        neighbors_.Replace({edge[1], trd_0}, ni[0], i_1);
        
        neighbors_.Replace({edge[0], trd_1}, ni[1], i_0);
        neighbors_.Replace({edge[1], trd_1}, ni[1], i_1);
        
        ReplaceNode<2>(ni[0], {{ i_0, i_1 }});
        ReplaceNode<2>(ni[1], {{ i_0, i_1 }});
    }
    
    void Insert(const Index index) {
        nodes_[ROOT]->Insert(index, ROOT, *this);
    }
    
    void Print(std::ostream& output) const {
        output << "Printing PointLocation object" << std::endl;
        output << "Node tree contains " << nodes_.size() << " elements" << std::endl;
        nodes_[ROOT]->Print(output, ROOT, nodes_);
    }
    
    void PrintNodes(std::ostream& output) const {
        for (Index i = 0; i < nodes_.size(); ++i) {
            output << "node: " << i << std::endl;
            output << nodes_[i]->trg;
        }
    }
    
    void PrintLeafNodes(std::ostream& output) const {
        for (Index i = 0; i < nodes_.size(); ++i) {
            if (!nodes_[i]->IsLeaf()) continue;
            output << "node: " << i << std::endl;
            output << nodes_[i]->trg;
        }
    }
    
    void PrintNeighbors(std::ostream& output) const {
        for (auto p : neighbors_) {
            output << "edge: " << p.first[0] << " " << p.first[1] << ", nodes: " << p.second[0] << " " << p.second[1] << "\n";
        }
    }

    std::vector<Triangle> Triangles() const {
        std::vector<Triangle> trgs;
        for (auto n : nodes_) {
            if (n->IsLeaf()) {
                trgs.push_back(n->trg);
            }
        }
        return trgs;
    }
    
    const AdjacentTrianglesIndex& neighbors() const {
        return neighbors_;
    } 
    
    const Triangle& triangle(Index i) const {
        return nodes_[i]->trg;
    }
    
    
    
    static constexpr Index ROOT = 0;
    AdjacentTrianglesIndex neighbors_;
    // sometimes we would need changes of pointer to be seen for multiple parents
    // that's why we are going to use this shit
    // easy to expand easy to refer 
    std::vector<Node*> nodes_;
    const IsInsideType* is_inside_;
    const IsOnSegmentType* is_on_segment_; 
    
    
    
    friend class Node;
    template<Count N> friend class Node_n; 
};


}
}
}

