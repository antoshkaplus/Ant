 
#ifndef BENTLEY_OTTMANN
#define BENTLEY_OTTMANN

#include "d2.h"

namespace ant {
    
namespace geometry {
    
namespace d2 {

// i modified algorithm a little bit.
// we insert all segments into SweepLine beforehand 
// to keep below and after correct for new elements
// and have visited array to keep track of what we should've added
// maybe there are to many if statements for this one
                      
                      
                      
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
        
        virtual void Handle() {};
        // could create order getter
    };
        
    struct EventComparator {
        bool operator()(const E_PTR& e_0, const E_PTR& e_1) const {
            auto& p_0 = e_0->order;
            auto& p_1 = e_1->order; 
            return p_0.x < p_1.x || (p_0.x == p_1.x && (p_0.y < p_1.y || (p_0.y == p_1.y && ))); 
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
        using EndpointEvent::order;
        using EndpointEvent::seg_index;
        
        RightEndpointEvent(BentleyOttmann& b, Index seg_index) 
        : EndpointEvent(b, seg_index, (*b.ps)[b.segs[seg_index].right]) {}
        
        void Handle() override {
            b.sweep_line.Print(std::cout);
            auto n = b.sweep_line.Find(*this).first;
            auto ab = n->above;
            auto be = n->below;
            b.sweep_line.Remove(n);
            // return Point, bool like in set
            if (!ab || !b.segs_visited[ab->seg_index] || !be) return; 
            std::pair<Point, bool> p = b.Intersection(ab, be);
            auto p_e = E_PTR{new Event(b, p.first)};
            if (p.second && b.event_queue.count(p_e) == 0) {
                Event *e = new IntersectionEvent(b, p.first, be->seg_index, ab->seg_index);
                b.event_queue.insert(E_PTR{e});
            }
        }
        
    };
    
    struct LeftEndpointEvent : EndpointEvent {
        using EndpointEvent::EndpointEvent;
        using EndpointEvent::seg_index;
        using EndpointEvent::b;
        
        LeftEndpointEvent(BentleyOttmann& b, Index seg_index) 
        : EndpointEvent(b, seg_index, (*b.ps)[b.segs[seg_index].left]) {}
        
        void Handle() override {
            b.segs_visited[seg_index] = true;
            auto n = b.sweep_line.Find(*this).first;
            auto ab = n->above;
            auto be = n->below;
            // could do it in a loop probably
            std::pair<Point, bool> p;
            if (ab && b.segs_visited[ab->seg_index]) {
                p = b.Intersection(n, ab);
                if (p.second) {
                    Event *e = new IntersectionEvent(b, p.first, n->seg_index, ab->seg_index);
                    b.event_queue.insert(E_PTR{e});
                }
            }
            if (be) {
                p = b.Intersection(n, be);
                if (p.second) {
                    Event *e = new IntersectionEvent(b, p.first, be->seg_index, n->seg_index);
                    b.event_queue.insert(E_PTR{e});
                }
            }
        }
    };
        
    struct IntersectionEvent : Event {
        using Event::Event;
        using Event::b;
        
        // first below, second above. they are ordered
        Index s_0, s_1;
        
        IntersectionEvent(BentleyOttmann& b, Point order, Index s_0, Index s_1)
            : Event(b, order), s_0(s_0), s_1(s_1) {}
        
        
        void Handle() override {
            // should i look them by intersection point ???
            b.intersections.push_back({s_0, s_1});
            LeftEndpointEvent e_0{b, s_0};
            // need to find endpointEvent
            auto s_0 = b.sweep_line.Find(e_0).first;
            b.sweep_line.SwapWithAbove(s_0);
            auto s_1 = s_0;
            s_0 = s_1->below;
            auto ab = s_1->above;
            auto be = s_0->below;
            std::pair<Point, bool> p;
            if (ab && b.segs_visited[ab->seg_index]) {
                p = b.Intersection(ab, s_1);
                // may already have intersection inside
                if (p.second && b.event_queue.count(E_PTR{new Event(b, p.first)}) == 0) {
                    Event *e = new IntersectionEvent(b, p.first, s_1->seg_index, ab->seg_index);
                    b.event_queue.insert(E_PTR{e});
                }
            }
            if (be) {
                p = b.Intersection(be, s_0);
                if (p.second && b.event_queue.count(E_PTR{new Event(b, p.first)}) == 0) {
                    Event *e = new IntersectionEvent(b, p.first, be->seg_index, s_0->seg_index);
                    b.event_queue.insert(E_PTR{e});
                }
            }
        }
        
    };

    // START SWEEP LINE

    struct Node;
    
    using N_PTR = std::shared_ptr<Node>;
    
    struct Node {
        Index seg_index;
        //Point order;
        // maybe use segment? structure where first element would be left 
        // and another point would be right
        
        // indices of right and left points
        N_PTR above;
        N_PTR below;
        
        Node(const BentleyOttmann& b, Index seg_index) 
        : seg_index(seg_index) {
            //order = (*b.ps)[b.segs[seg_index].left];
        }
    };

    struct SweepLine {
    private:
        
        struct Comparator {
            const BentleyOttmann& b;
        
            Comparator(const BentleyOttmann& b) : b(b) {}
        
            bool operator()(const N_PTR& n_0, const N_PTR& n_1) const {
                auto ps = *(b.ps);
                auto& p_0 = ps[b.segs[n_0->seg_index].left];
                auto& p_1 = ps[b.segs[n_1->seg_index].left];
                return p_0.y < p_1.y || (p_0.y == p_1.y && p_0.x < p_1.x);
            }
        };
    
        // need to make order // 
        std::set<N_PTR, Comparator> nodes;
        
    public:
        
        SweepLine(const BentleyOttmann& b) : nodes(Comparator(b)) {}
        
        // adding first, that means that can look in the tree for above and below
        N_PTR Add(const EndpointEvent& e) {
             auto n = new Node(e.b, e.seg_index);
             // can get left and right by edge index
             n->seg_index = e.seg_index;
             typename decltype(nodes)::iterator it_t, it = nodes.insert(N_PTR{n}).first;
             if ((it_t = std::next(it)) != nodes.end()) {
                (**it).above = *it_t; 
                (**it_t).below = *it;
             }
             if (it != nodes.begin()) {
                it_t = std::prev(it);
                (**it).below = *it_t;
                (**it_t).above = *it;
             }
             return *it;
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
            assert(it != nodes.end());
            auto ab = (**it).above;
            auto be = (**it).below;
            if (ab) {
                ab->below = be;
            }
            if (be) {
                be->above = ab;
            }
            nodes.erase(it);
        }
        
        // n_0, n_1 ordered in sequence
        void SwapWithAbove(N_PTR n_0) {
            N_PTR n_1 = n_0->above;
            if (n_0->below) {
                n_0->below->above = n_1;
            }
            if (n_1->above) {
                n_1->above->below = n_0;
            }
            n_0->above = n_1->above;
            n_1->below = n_0->below;
            n_0->below = n_1;
            n_1->above = n_0;
        }
        
        
        void Print(std::ostream& out) {
            for (auto n : nodes) {
                out << n->seg_index << std::endl;
            }
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
    std::vector<bool> segs_visited;
    
    std::pair<Point, bool> Intersection(N_PTR n_0, N_PTR n_1) {
        return (*inter)(n_0->seg_index, n_1->seg_index);
    }
    
    
public:

    BentleyOttmann() : sweep_line(*this) {}

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
        this->inter = &inter;
        segs_visited.resize(segs.size());
        std::fill(segs_visited.begin(), segs_visited.end(), false);
        
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
            auto left = new LeftEndpointEvent(*this, i);
            sweep_line.Add(*left);
            event_queue.insert(E_PTR{(Event*)left});
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