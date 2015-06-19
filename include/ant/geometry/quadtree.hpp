//
//  quadtree.hpp
//  ANT
//
//  Created by Anton Logunov on 6/18/15.
//
//

#ifndef ANT_quadtree_hpp
#define ANT_quadtree_hpp

#include <array>
#include <algorithm>

#include "ant/geometry/d2.hpp"


namespace ant {

namespace geometry {
    
namespace d2 {
    
namespace f {
    
    
class Quadtree {
    
    const static Count VALUE_NODE_CAP = 16;
    const static Count QUARTER_COUNT = 4;
    
    struct R {
        Point p;
        double d;
    };
    
    struct Data {
        
        Point topleft;
        Point botright;
        
        Data() {}
        Data(const Point& topleft, const Point& botright)
            : topleft(topleft), botright(botright) {}
        
        
        double Radius() const {
            return topleft.Distance(botright)/2.;
        }
        
        Point Center() const {
            return (topleft + botright)/2;
        }
        
        std::array<Data, QUARTER_COUNT> Quarters() const {
            Point c = Center();
            return {{ Q_0(c), Q_1(c), Q_2(c), Q_3(c) }};
        } 
        
        std::array<Point, QUARTER_COUNT> QuarterCenters() const {
            auto c = (topleft + botright) / 2;
            return {{ Q_0(c).Center(),Q_1(c).Center(),
                      Q_2(c).Center(),Q_3(c).Center() }};
        }
        
        Data Q_0(const Point& c) const {
            return Data{topleft, c};
        }
        
        Data Q_1(const Point& c) const {
            return Data{Point{c.x, topleft.y}, Point{botright.x, c.y}};
        }
        
        Data Q_2(const Point& c) const {
            return Data{Point{topleft.x, c.y}, Point{c.x, botright.y}};
        }
        
        Data Q_3(const Point& c) const {
            return Data{c, botright};
        }
        
        std::tuple<Index, Data> Quarter(const Point& p) const {
            auto c = Center();
            if (c.x >= p.x) {
                if (c.y >= p.y) {
                    return {0, Q_0(c)};
                } else {
                    return {2, Q_2(c)};
                }
            } else {
                if (c.y >= p.y) {
                    return {1, Q_1(c)};
                } else {
                    return {3, Q_3(c)};
                }
            }
        }
    };
    
    // Node support classes
    
    class Node {
    public:
        // d : current minimum distance
        // returns possible new minimum distance or something bigger     
        virtual void NN(const Point& p, 
                const Data& data, 
                R& res) {}
        
        // node : this address, so that would be possible to update 
        virtual void Insert(const Point& p, 
                    const Data& data, 
                    Node** node);
        
        virtual void Remove(const Point& p, const Data& data) {}
        
        virtual ~Node() {}
        
    protected:
        bool NN_ShouldConsider(const Point& p, 
                               const Data& data, 
                               double d) {
            double r = data.Radius();
            Point c = data.Center();
            return Circle(p, d).Overlap(Circle(c, r));
        }
    };

    class ParentNode : public Node {
    private:
        std::array<Node*, QUARTER_COUNT> nodes;
        
    public:
        ParentNode() {
            for (auto& n : nodes) {
                n = new Node();
            }
        }
        
        virtual void Insert(const Point& p, 
                    const Data& data, 
                    Node** node) override {
            Index i;
            Data i_data;
            std::tie(i, i_data) = data.Quarter(p);
            nodes[i]->Insert(p, i_data, &nodes[i]);
        }
    
        virtual void NN(const Point& p, 
                const Data& data, 
                R& r) override {
                
            if (!NN_ShouldConsider(p, data, r.d)) return;
            auto qcs = data.QuarterCenters();
            std::array<Index, QUARTER_COUNT> inds = {{0, 1, 2, 3}};
            std::array<double, QUARTER_COUNT> dist;
            std::transform(inds.begin(), inds.end(), dist.begin(), [&] (Index i) {
                return p.Distance(qcs[i]);
            });
            std::sort(inds.begin(), inds.end(), [&](Index i_0, Index i_1) {
                return dist[i_0] < dist[i_1];
            });
            auto qs = data.Quarters();
            for (Index i : inds) {
                nodes[i]->NN(p, qs[i], r);
            }
        }
        
        virtual void Remove(const Point& p, const Data& data) override {
            Index i;
            Data i_data;
            std::tie(i, i_data) = data.Quarter(p);
            nodes[i]->Remove(p, i_data);
        }
                
        // this one is actually search for particular point
//        pair<const Point&, bool> find(
//                                    const Point& p, 
//                                    const Data& data) {
//            Index i;
//            tie(i, topleft, botright) = Quater(p, topleft, botright);
//            return nodes[i]->NN(p, topleft, botright);
//        }
    };

    template<Count CAP>
    class ValueNode : public Node {
    private:
        Point values[CAP];
        Count count = 0;
    
    public:

        // can use -1 in double or max distance would be better
        virtual void NN(const Point& p, 
                const Data& data, 
                R& r) override {
            for (Index i = 0; i < count; ++i) {
                double d;
                if ((d = p.Distance(values[i])) < r.d) {
                    r.d = d;
                    r.p = values[i]; 
                } 
            }
        }
        
        virtual void Insert(const Point& p, 
                            const Data& data, 
                            Node** node) override {
            if (count != CAP) {
                values[count++] = p;
                return;
            } 
            // this is another node
            *node = new ParentNode();
            for (Index i = 0; i < CAP; ++i) {
                (**node).Insert(values[i], data, node);
            }
            (**node).Insert(p, data, node);
            delete this;
        }
        
        virtual void Remove(const Point& p, const Data& data) override {
             if (std::remove(values, values+count, p) - values != count) {
                 --count;
             }
        }
    };  
    
    // Quadtree class
    
    Node *root_; 
    Data data_;
    
    
public:
    Quadtree(Point topleft, Point botright) : data_(topleft, botright) {
        root_ = new Node();
    }
    
    void Insert(const Point& p) {
        root_->Insert(p, data_, &root_);
    }
    
    // should 
    Point NearestNeighbor(const Point& p) const {
        R r;
        r.d = data_.Radius();
        root_->NN(p, data_, r);
        return r.p;
    }
    
    void Remove(const Point& p) {
        root_->Remove(p, data_);
    }
};


}

}

}

}




#endif
