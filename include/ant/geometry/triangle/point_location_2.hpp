//
//  point_location_2.hpp
//  Ant
//
//  Created by Anton Logunov on 7/16/15.
//
//

#ifndef Ant_point_location_2_hpp
#define Ant_point_location_2_hpp

#include "triangle.hpp"

namespace ant {
namespace geometry {
namespace triangle {


template<class IsInsideType, class IsOnSegmentType>
class PointLocation_2 {

    using EdgeIndex = Index;
    using NodeIndex = Index;

    using Edges = std::array<EdgeIndex, 3>;
    using PL = PointLocation_2<IsInsideType, IsOnSegmentType>;
    using Neighbors = std::vector<std::array<NodeIndex, 2>>;
    
    class Node;
    using Nodes = std::vector<Node*>;
    
    using NI_2 = std::array<NodeIndex, 2>;
    using NI_3 = std::array<NodeIndex, 3>;
    
    template<Count N> class Node_n;
    using Node_2 = Node_n<2>;
    using Node_3 = Node_n<3>;
    
    static constexpr Index ROOT = 0;
    
    // Leaf
    class Node {
    public:
        
        Node(const Triangle& trg, const Edges& edges)
            : trg(trg), edges(edges) {}
        
        virtual bool IsLeaf() const {
            return true;
        }
         
        Index OppositeVertex(EdgeIndex e) {
            for (Index i = 0; i < 3; ++i) {
                if (e == edges[i]) {
                    return trg[(i+2)%3];
                }
            }
        }
        
        struct Edge Edge(EdgeIndex e) {
            for (Index i = 0; i < 3; ++i) {
                if (e == edges[i]) {
                    return {{trg[i], trg[(i+1)%3] }};
                }
            }
        }
        
        virtual void Insert(Index index, NodeIndex self, PL& pl) {
            struct Edge edge;
            bool is_on_edge;
            tie(edge, is_on_edge) = trg.OnEdge(index, (*pl.is_on_segment_));
            if (is_on_edge) {
                InsertOnEdge(index, edge, pl);
            } else {
                InsertInside(index, self, pl);
            }   
        } 
        
        virtual void Print(std::ostream& output, Index self, const Nodes& ns) const {
            output << "node: " << self << std::endl;
            output << "triangle: " << trg << std::endl;
            // need to print array of edges too
        }
        
        virtual ~Node() {}
        
    private:
        
        Index edge_index(const struct Edge& edge) {
            return 0;
        }
        
        // add arguments that are needed
        // need to delete old nodes
        void InsertOnEdge(Index index, const struct Edge& edge, PL& pl) {
            // could wrap everything inside loop of two iterations
            // but this implementation is more explicit
            auto& neighbors = pl.neighbors_;
            auto& nodes = pl.nodes_;
            Index e_i = edge_index(edge);
            auto ni = neighbors[e_i];
            
            // need to reuse e_i
            
            // middle edge indices
            auto e_0 = e_i;
            auto e_1 = pl.NewEdge();
            
            // base edge indices
            auto ee_0 = pl.NewEdge();
            auto ee_1 = pl.NewEdge();
            
            auto n_0 = nodes[ni[0]];
            auto n_1 = nodes[ni[1]];
            
            auto trd_0 = n_0->trg.Third(edge);
            auto trd_1 = n_1->trg.Third(edge);
            
            // first big triangle split
            auto e_0_0 = edge_index({edge[0], trd_0});
            auto e_0_1 = edge_index({edge[1], trd_0});
            
            auto n_0_0 = new Node(Triangle(index, edge[0], trd_0), {{e_0_0, e_0, ee_0}});
            auto n_0_1 = new Node(Triangle(index, edge[1], trd_0), {{e_0_1, e_0, ee_1}});
            NodeIndex i_0_0 = nodes.size();
            nodes.push_back(n_0_0);
            NodeIndex i_0_1 = nodes.size();
            nodes.push_back(n_0_1);
            
            pl.ReplaceNode<2>(ni[0], NI_2{{ i_0_0, i_0_1 }});
            
            pl.ReplaceEdgeNode(e_0_0, ni[0], i_0_0);
            pl.ReplaceEdgeNode(e_0_1, ni[0], i_0_1);
            
            neighbors[e_0] = {{i_0_0, i_0_1}};
            
            // second big triangle split
            auto e_1_0 = edge_index({edge[0], trd_1});
            auto e_1_1 = edge_index({edge[1], trd_1});
            
            auto n_1_0 = new Node(Triangle(index, edge[0], trd_1), {{e_1_0, e_1, ee_0}});
            auto n_1_1 = new Node(Triangle(index, edge[1], trd_1), {{e_1_1, e_1, ee_1}});
            NodeIndex i_1_0 = nodes.size();
            nodes.push_back(n_1_0);
            NodeIndex i_1_1 = nodes.size();
            nodes.push_back(n_1_1);
            
            pl.ReplaceNode<2>(ni[1], NI_2{{ i_1_0, i_1_1 }});
            
            pl.ReplaceEdgeNode(e_1_0, ni[1], i_1_0);
            pl.ReplaceEdgeNode(e_1_1, ni[1], i_1_1);
            
            neighbors[e_1] = {{i_1_0, i_1_1}};
        }
        
        // need triangle index
        void InsertInside(Index index, NodeIndex self, PL& pl) {
            // should probably make stuff explictily too
            auto& nodes = pl.nodes_;
            // we have 3 new edges...
            // we have two provide two of them along with the initial on
            array<Index, 3> e = {pl.NewEdge(), pl.NewEdge(), pl.NewEdge()};
            // they are ordered with triangle....
            auto te = edges;
            for (Index k = 0; k < 3; ++k) {
                nodes.push_back(new Node(Triangle(trg[k], trg[k+1], index), te[k], e[k], e[k+1]));
                NodeIndex i_2 = nodes.size()-1;
                pl.ReplaceEdgeNode(te[k], self, i_2);
                // for this one you can use another loop of three... but should start with -1... when inserting adjacent guys // or just let it be
                pl.InsertEdge(e[k], i_2);
                pl.InsertEdge(e[k+1], i_2);
            }
            NodeIndex last = nodes.size()-1;
            pl.ReplaceNode<3>(self, NI_3{{last, last-1, last-2}});
        }
        
        Triangle trg;
        // edge => trg : 0 => (0, 1), 1 => (1, 2), 2 => (0, 2)
        Edges edges; 
    };
    friend class Node;
    
    template<Count N>
    class Node_n : public Node {
    public:
        std::array<NodeIndex, N> children;
        
        Node_n(Triangle& trg, const std::array<NodeIndex, N>& ns) : Node(trg) {
            children = ns;
        }
        
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
            ant::Print<NodeIndex, N>(output, children);
            for (auto ch : children) {
                ns[ch]->Print(output, ch, ns);
            }
        } 
    };
    template<Count N> friend class Node_n;         
            
            
    EdgeIndex NewEdge() {
        EdgeIndex i = neighbors_.size();
        neighbors_.push_back();
        return i;
    }
            
    // those are different operations we use on PointLocation members to 
    // relax Node and Node_n load 
    void ReplaceEdgeNode(const Edge& e, Index from, Index to) {
        auto& ni = neighbors_[e];
        assert(ni[0] == from || ni[1] == from);
        std::swap(ni[0] == from ? ni[0] : ni[1], to);
    }
        
    void InsertEdge(const Edge& e, NodeIndex i_0, NodeIndex i_1) {
        neighbors_[e] = {{i_0, i_1}};
    }
    
    void InsertEdge(EdgeIndex& e, NodeIndex i) {
        
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
    
    PointLocation_2(const Triangle& trg, 
                    Count point_count, 
                    const IsInsideType& is_inside, 
                    const IsOnSegmentType& is_on_segment) {
        
    }
    
            
    void Insert(const Index index) {
        nodes_[ROOT]->Insert(index, ROOT, *this);
    }
    
    void Flip(Index e_i) {
        NI_2 ni = neighbors_[e_i];
        auto& n_0 = nodes_[ni[0]];
        auto& n_1 = nodes_[ni[1]]; 
        
        //RemoveEdge(e_i);
        
        // third should be for node itself
        auto trd_0 = n_0->OppositeVertex(e_i);
        auto trd_1 = n_1->OppositeVertex(e_i);
        
        //std::cout << edge[0] << " " << edge[1] << " " << trd_0 << " " << trd_1 << std::endl;
        
        // need to provide other triangles
        auto edge = n_0->Edge(e_i);
        nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[0]), ));
        Index i_0 = nodes_.size()-1;
        nodes_.push_back(new Node(Triangle(trd_0, trd_1, edge[1])));
        Index i_1 = nodes_.size()-1;
        
        // reuse e_i
        InsertEdge({trd_0, trd_1}, i_0, i_1);
        
        ReplaceEdgeNode({edge[0], trd_0}, ni[0], i_0);
        ReplaceEdgeNode({edge[1], trd_0}, ni[0], i_1);
        
        ReplaceEdgeNode({edge[0], trd_1}, ni[1], i_0);
        ReplaceEdgeNode({edge[1], trd_1}, ni[1], i_1);
        
        ReplaceNode<2>(ni[0], {{ i_0, i_1 }});
        ReplaceNode<2>(ni[1], {{ i_0, i_1 }});
    }


            
    // inside triangle there are 
    // Edge => Index 
    std::vector<Node*> nodes_;
    // index edge
    std::vector<std::array<NodeIndex, 2>> neighbors_;
    const IsInsideType* is_inside_;
    const IsOnSegmentType* is_on_segment_; 
    
};


}
}
}

#endif
