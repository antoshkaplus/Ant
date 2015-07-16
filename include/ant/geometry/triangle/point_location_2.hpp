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

template<class IsInsideType, class IsOnSegmentType>
class PointLocation_2 {

    // inside triangle there are 
    // Edge => Index 
    std::vector<Node*> nodes_;
    // index edge
    std::vector<std::array<Index, 2>> neighbors_;
    
    
    // Leaf
    class Node {
    public:
        Triangle trg;
        
        Node(const Triangle& trg) {
            this->trg = trg;
        }
        
        virtual bool IsLeaf() const {
            return true;
        }
        
        virtual void Insert(Index index, NodeIndex self, PL& pl) {
            // we start from zero index triangle...
            // sometimes they have children... 
            // so basically inside node we have triangle, children, 
            // and indices of the edges... => only when it's leaf node
            
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
        
        Index edge_index(const Edge& edge) {
            return 0
        }
        
        // add arguments that are needed
        // need to delete old nodes
        void InsertOnEdge(Index index, const Edge& edge, PL& pl) {
            // could wrap everything inside loop of two iterations
            // but this implementation is more explicit
            auto& neighbors = pl.neighbors_;
            auto& nodes = pl.nodes_;
            Index e_i = edge_index(edge);
            auto ni = neighbors[e_i];
            
            pl.RemoveEdge(e_i);
            
            // middle edge indices
            e_0 = NewEdge();
            e_1 = NewEdge();
            // base edge indices
            ee_0 = NewEdge();
            ee_1 = NewEdge();
            
            n_0 = nodes[ni[0]];
            n_1 = nodes[ni[1]];
            
            trd_0 = n_0->trg.Third(edge);
            trd_1 = n_1->trg.Third(edge);
            
            // first big triangle split
            auto e_0_0 = edge_index({edge[0], trd_0});
            auto e_0_1 = edge_index({edge[1], trd_0});
            
            auto n_0_0 = new Node(Triangle(index, edge[0], trd_0), e_0_0, e_0, ee_0);
            auto n_0_1 = new Node(Triangle(index, edge[1], trd_0), e_0_1, e_0, ee_1);
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
            
            auto n_1_0 = new Node(Triangle(index, edge[0], trd_1), e_1_0, e_1, ee_0);
            auto n_1_1 = new Node(Triangle(index, edge[1], trd_1), e_1_1, e_1, ee_1);
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
            array<Index, 3> e = {NewEdge(), NewEdge(), NewEdge()};
            // they are ordered with triangle....
            auto te = edge_indices()
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
        
    };
    

    



    
    PointLocation_2(const Triangle& trg, 
                    Count point_count, 
                    const IsInsideType& is_inside, 
                    const IsOnSegmentType& is_on_segment) {
        
    }


};


#endif
