
#ifndef BENTLEY_OTTMANN
#define BENTLEY_OTTMANN

#include "d2.h"

namespace ant {
    
namespace geometry {
    
namespace d2 {
  
                      
// pass something with call operator 
// that takes two segment indices as arguments   
template<typename Point, typename Intersect>
class BentleyOttmann {
    
    struct Event;
    
    using P = std::pair<Index, Index>;
    using E_PTR = std::shared_ptr<Event>;
    
    // ordered pair
    struct OP {
        Index left;
        Index right;
        
        OP() {}
        OP(const P& p) : left(p.first), right(p.second) {}
        void set(const P& p) {
            left = p.first;
            right = p.second;
        }
    };
    
    struct Event {
        BentleyOttmann& b;
        Point order;
        
        Event(BentleyOttmann& b, Point order) : b(b), order(order) {}
        
        virtual void Handle() = 0;
        // could create order getter
    };
    
    struct EmptyEvent : Event {
        using Event::Event;
        
        void Handle() override {}
    };
    
    struct EventComparator {
        bool operator()(const E_PTR& e_0, const E_PTR& e_1) {
            auto& p_0 = e_0->order;
            auto& p_1 = e_1->order; 
            return p_0.x < p_1.x || (p_0.x == p_1.x && p_0.y < p_1.y); 
        }
    };
    
    
    struct EndpointEvent : Event {
        using Event::b;
        
        Index seg_index;
        
        EndpointEvent(BentleyOttmann& b, Index seg_index, Point order) 
        : Event(b, order), seg_index(seg_index) {}
    };
    
    
    struct RightEndpointEvent : EndpointEvent {
        using EndpointEvent::EndpointEvent;
        using EndpointEvent::b;
        
        RightEndpointEvent(BentleyOttmann& b, Index seg_index) 
        : EndpointEvent(b, seg_index, (*b.ps)[b.segs[seg_index].right]) {}
        
        void Handle() override {
            auto n = b.sweep_line.Add(*this);
            auto ab = n->above;
            auto be = n->below;
            b.sweep_line.Remove(n);
            // return Point, bool like in set
            std::pair<Point, bool> p = b.Intersection(ab, be);
            auto p_e = E_PTR{new EmptyEvent(b, p.first)};
            if (p.second && b.event_queue.count(p_e) == 0) {
                Event *e = new IntersectionEvent(p.first, ab->seg_index, be->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
        }
        
    };
    
    struct LeftEndpointEvent : EndpointEvent {
        using EndpointEvent::EndpointEvent;
        using EndpointEvent::b;
        
        LeftEndpointEvent(BentleyOttmann& b, Index seg_index) 
        : EndpointEvent(b, seg_index, (*b.ps)[b.segs[seg_index].left]) {}
        
        void Handle() override {
            auto n = b.sweep_line.Add(*this);
            auto ab = n->above;
            auto be = n->below;
            // could do it in a loop probably
            std::pair<Point, bool> p;
            p = b.Intersection(n, ab);
            if (p.second) {
                Event *e = new IntersectionEvent(b, p.first, n->seg_index, ab->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
            p = b.Intersection(n, be);
            if (p.second) {
                Event *e = new IntersectionEvent(b, p.first, n->seg_index, be->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
        }
    };
        
    struct IntersectionEvent : Event {
        using Event::Event;
        using Event::b;
        
        Index s_0, s_1;
        
        IntersectionEvent(BentleyOttmann& b, Point order, Index s_0, Index s_1)
            : Event(b, order), s_0(s_0), s_1(s_1) {}
        
        
        void Handle() override {
            // should i look them by intersection point ???
            b.intersections.push_back({s_0, s_1});
            EndpointEvent e_0{b, s_0, b.ps[b.segs[s_0].left]};
            EndpointEvent e_1{b, s_1, b.ps[b.segs[s_1].left]};
            // need to find endpointEvent
            auto s_0 = b.sweep_line.Find(e_0).first;
            auto s_1 = b.sweep_line.Find(e_1).first;
            swap(s_0->order, s_1->order);
            auto a = s_1->above;
            auto b = s_0->below;
            std::pair<Point, bool> p;
            p = b.Intersection(a, s_1);
            if (p.second && b.event_queue.count(p.first) == 0) {
                Event *e = new IntersectionEvent(p.first, a->seg_index, b->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
            p = b.Intersection(b, s_0);
            if (p.second && b.event_queue.count(p.first) == 0) {
                Event *e = new IntersectionEvent(p.first, a->seg_index, b->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
        }
        
    };

    // START SWEEP LINE

    struct Node;
    
    using N_PTR = std::shared_ptr<Node>;
    
    struct Node {
        Index seg_index;
        Point order;
        // maybe use segment? structure where first element would be left 
        // and another point would be right
        
        // indices of right and left points
        N_PTR above;
        N_PTR below;
        
        Node(const BentleyOttmann& b, Index seg_index) 
        : seg_index(seg_index) {
            order = (*b.ps)[b.segs[seg_index].left];
        }
    };

    struct SweepLine {
    private:
        
        struct Comparator {
            bool operator()(const N_PTR& n_0, const N_PTR& n_1) {
                auto& p_0 = n_0->order;
                auto& p_1 = n_1->order; 
                return p_0.y < p_1.y || (p_0.y == p_1.y && p_0.x < p_1.x);
            }
        };
    
        // need to make order // 
        std::set<N_PTR, Comparator> nodes{Comparator()};
        
    public:
        
        N_PTR Add(const EndpointEvent& e) {
             auto n = new Node(e.b, e.seg_index);
             // can get left and right by edge index
             n->seg_index = e.seg_index;
             typename decltype(nodes)::iterator it_t, it = nodes.insert(N_PTR(n)).first;
             if ((it_t = std::next(it)) != nodes.end()) {
                (**it).above = *it_t; 
             }
             if (it != nodes.begin()) {
                (**it).below = *std::prev(it);
             }
             return N_PTR{n};
        }
        
        std::pair<N_PTR, bool> Find(const EndpointEvent& e) {
            std::pair<N_PTR, bool> p;
            auto n = nodes.find(N_PTR(new Node(e.b, e.seg_index)));
            if (n == nodes.end()) {
                p.second = false;
            } else {
                p.first = *n;
                p.second = true;
            }
            return p;
        }
        
        void Remove(N_PTR n) {
            auto it = nodes.find(n);
            // need to update above and below of it
            // before removing
            nodes.erase(it);
        }
    };
    
    // END SWEEP LINE
    
    SweepLine sweep_line;
    std::set<E_PTR, EventComparator> event_queue{EventComparator()};
    std::vector<OP> segs;
    const Intersect *inter;
    // another ways of doing it? // pass it as parameter
    // should return segs inds and point
    std::vector<P> intersections;
    const std::vector<Point>* ps; 
    
    
    std::pair<Point, bool> Intersection(N_PTR n_0, N_PTR n_1) {
        return (*inter)(n_0->seg_index, n_1->seg_index);
    }
    
    
public:
    // having segments isn't cool at all
    // should be inside cpp file
    // segs are pairs of point indices
    // 
    // return  
    
    // should not we just copy everything from here????
    std::vector<P> FindIntersections(const std::vector<Point>& ps,
                                     const std::vector<P>& segs,
                                     const Intersect& inter) {
        this->ps = &ps;
        auto should_swap = [&](Index i_0, Index i_1) {
            return ps[i_0].x > ps[i_1].x;
        };
        auto& segs_op = this->segs;
        segs_op.resize(segs.size());
        for (int i = 0; i < segs.size(); ++i) {
            segs_op[i].set(segs[i]);
            if (should_swap(segs_op[i].left, segs_op[i].right)) {
                std::swap(segs_op[i].left, segs_op[i].right);
            }
        } 
        // queue initialization
        for (int i = 0; i < segs_op.size(); ++i) {
            event_queue.insert(E_PTR{(Event*)new LeftEndpointEvent(*this, i)});
            event_queue.insert(E_PTR{(Event*)new RightEndpointEvent(*this, i)});
        }
        // algorithm itself
        while (!event_queue.empty()) {
            auto it = event_queue.begin();
            (**it).Handle();
            event_queue.erase(it);
        }
        return intersections;
    }


};


} // end namespace d2

} // end namespace geometry

} // ant


#endif