
#include <array>


#include "ant/geometry/d2.hpp"


using namespace std;
using namespace ant;
using namespace ant::geometry::d2::i;





// put inside indices // returns if point inside
template<typename InsideTriangle>
struct PointLocation {
    
    struct Node {
        Count sz = 0;
        Node* ns[3];
        array<Index, 3> ps;
        
        Node(Index i_0, Index i_1, Index i_2) {
            ps[0] = i_0;
            ps[1] = i_1;
            ps[2] = i_2;
        }
    };
    
    Node* root = nullptr;
    
    Count point_count_; 
    InsideTriangle& inside_triangle_; 
    
    PointLocation(Count point_count, InsideTriangle& inside_triangle)
        : point_count_(point_count), inside_triangle_(inside_triangle) {}
    
    void Init(Index i_0, Index i_1, Index i_2) {
        root = new Node(i_0, i_1, i_2);
    }
    
    void Add(Index i) {
        Node* n = root;
        Node* ns[3];
        Count sz;
        while (n->sz != nullptr) {
            sz = 0;
            for (Index i = 0; i < n->sz; ++i) {
                // should be able to take array of points
                if (inside_triangle_(n->ns[i])) {
                    ns[sz++] = n->ns[i];
                }
            }
            // on some edge
            if (sz > 1) {
                // need to find to common points in ns[0] and ns[1]
                // after that go father down until find last child with this kind of edge
            } else {
                // equal to 1
                n = ns[0];
            }
            
        }
        
    
    }
    
    
    
    

    
};




int main(int argc, char **argv) {


}

