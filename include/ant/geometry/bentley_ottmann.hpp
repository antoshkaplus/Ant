#pragma once

#include <iostream>
#include <array>
#include <map>

#include "d2.hpp"

namespace ant::geometry::d2 {

// Algo running time:
// O(n log n + I log n)
// n - number of segments
// I - number of intersections

// should work on implementation of general case when 
// there are multiple points intersect in one line or 
// ends lie on each other and so forth
// should be ok to use multiset
                      
// taking real values is really scary


// pass something with call operator 
// that takes two segment indices as arguments   
template<typename Point, typename Intersect>
class BentleyOttmann {
    
    struct Event;
    
    using P = std::array<Index, 2>;
    // why the heck shared_ptr is here???
    using E_PTR = std::shared_ptr<Event>;
    
    // ordered pair
    struct OP {
        Index left;
        Index right;
        
        OP() {}
        OP(const P& p) : left(p[0]), right(p[1]) {}
        void set(const P& p) {
            left = p[0];
            right = p[1];
        }
    };
    
    struct Event {
        BentleyOttmann& b;
        Point order;

        // don't forget to initialize
        Event(BentleyOttmann& b) : b(b) {}
        
        Event(BentleyOttmann& b, Point order)
            : b(b), order(order) {}
        
        void set(const Point& order) {
            this->order = order;
        }
        
        virtual void Handle() {};
        // could create order getter
    };


    struct EventComparator {
        // what is this thing??? wtf???
        Index order(const E_PTR& e) const {
            if (dynamic_cast<LeftEndpointEvent*>(e.get())) {
                return 0;
            } else if (dynamic_cast<IntersectionEvent*>(e.get())) {
                return 1;
            } else {
                return 2;
            }
        }
        
        
        bool operator()(const E_PTR& e_0, const E_PTR& e_1) const {
            auto& p_0 = e_0->order;
            auto& p_1 = e_1->order; 


            return p_0.x < p_1.x 
                    || (p_0.x == p_1.x && (p_0.y < p_1.y));
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
            auto above = b.sweep_line.Above(seg_index);
            auto below = b.sweep_line.Below(seg_index);
            b.sweep_line.Remove(seg_index);
            // return Point, bool like in set
            if (above == seg_index || below == seg_index) return; 
            std::pair<Point, bool> p = b.Intersection(above, below);
            if (p.second) {
                auto e = new IntersectionEvent(b, p.first, below, above);
                E_PTR ep{e};
                if (b.event_queue.count(ep) == 0) {
                    b.event_queue.insert(ep);
                }
            }
        }
    };
    
    struct LeftEndpointEvent : EndpointEvent {
        using EndpointEvent::EndpointEvent;
        using EndpointEvent::seg_index;
        using EndpointEvent::b;
        using EndpointEvent::order;
        
        LeftEndpointEvent(BentleyOttmann& b, Index seg_index) 
        : EndpointEvent(b, seg_index, (*b.ps)[b.segs[seg_index].left]) {}
        
        void Handle() override {
            b.segs_visited[seg_index] = true;
            b.sweep_line.Add(seg_index, order);
            auto above = b.sweep_line.Above(seg_index);
            auto below = b.sweep_line.Below(seg_index);
            // could do it in a loop probably
            std::pair<Point, bool> p;
            if (above != seg_index) {
                p = b.Intersection(seg_index, above);
                if (p.second) {
                    Event *e = new IntersectionEvent(b, p.first, seg_index, above);
                    b.event_queue.insert(E_PTR{e});
                }
            }
            if (below != seg_index) {
                p = b.Intersection(seg_index, below);
                if (p.second) {
                    Event *e = new IntersectionEvent(b, p.first, below, seg_index);
                    b.event_queue.insert(E_PTR{e});
                }
            }
        }
    };
        
    struct IntersectionEvent : Event {
        using Event::Event;
        using Event::b;
        using Event::order;
        
        // first below, second above. they are ordered
        Index s_0, s_1;
        
        IntersectionEvent(BentleyOttmann& b, Point order, Index s_0, Index s_1)
                : Event(b), s_0(s_0), s_1(s_1) {
            if (s_0 > s_1) std::swap(s_0, s_1);
            Event::set(order);
        }
        
        
        void Handle() override {
            // should i look them by intersection point ???
            //assert(b.sweep_line.Above(s_0) == s_1);
            b.intersections.push_back({{s_0, s_1}});
            b.sweep_line.Swap(s_0, s_1, order);
            std::swap(s_0, s_1);
            auto above = b.sweep_line.Above(s_1);
            auto below = b.sweep_line.Below(s_0);
            std::pair<Point, bool> p;
            if (above != s_1) {
                p = b.Intersection(above, s_1);
                // may already have intersection inside
                if (p.second) {
                    auto e = new IntersectionEvent(b, p.first, s_1, above);
                    // should check if this point is actually father 
                    E_PTR ep{e};
                    if (b.event_queue.count(ep) == 0) {
                        b.event_queue.insert(ep);
                    }
                }
            }
            if (below != s_0) {
                p = b.Intersection(below, s_0);
                if (p.second) {
                    Event *e = new IntersectionEvent(b, p.first, below, s_0);
                    E_PTR ep{e};
                    if (b.event_queue.count(ep) == 0) {
                        b.event_queue.insert(ep);
                    }
                }
            }
        }
        
    };

    // START SWEEP LINE

    struct SweepLine {
    private:
        
        struct Comparator {
            bool operator()(const f::Point& p_0, const f::Point& p_1) const {
                return p_0.y < p_1.y || (p_0.y == p_1.y && p_0.x < p_1.x);
            }
        };
        
        
        // need to make order // 
        std::multimap<f::Point, Index, Comparator> nodes;
        using It = typename decltype(nodes)::iterator;
        // level segments
        // may want to use list for uicker deletion
        std::vector<It> segs;
        
        
    public:
        
        SweepLine() : nodes(Comparator()) {}

        SweepLine(Count segment_count)
            : nodes(Comparator()), segs(segment_count) {}
            
        void Reset(Count segment_count) {
            nodes.clear();
            segs.resize(segment_count);
        }    
        
        // adding first, that means that can look in the tree for above and below
        void Add(Index seg_index, const f::Point& order) {
            segs[seg_index] = nodes.insert({order, seg_index}); 
        }
        
        void Remove(Index seg_index) {
            nodes.erase(segs[seg_index]);
        }
        
        // n_0, n_1 ordered in sequence
        void Swap(Index s_0, Index s_1, const f::Point& order) {
            nodes.erase(segs[s_0]);
            nodes.erase(segs[s_1]);
            segs[s_1] = nodes.insert({order, s_1});
            segs[s_0] = nodes.insert({order, s_0});
        }
        
        Index Above(Index seg_index) {
            It next = std::next(segs[seg_index]);
            return next == nodes.end() ? seg_index : next->second; 
        }
        
        Index Below(Index seg_index) {
            return segs[seg_index] == nodes.begin() ? 
                        seg_index : std::prev(segs[seg_index])->second;
        }
        
        void Print(std::ostream& out) {
            for (auto n : nodes) {
                out << n.second << " ";
            }
            out << std::endl;
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
    
    std::pair<Point, bool> Intersection(Index s_0, Index s_1) {
        return (*inter)(s_0, s_1);
    }
    
public:

    BentleyOttmann() {}

    // having segments isn't cool at all
    // should be inside cpp file
    // segs are pairs of point indices
    // 
    // return  
    
    // should not we just copy everything from here????


    // what if I want to process them one by one and terminate at some point...
    // is it possible???
    std::vector<P> FindIntersections(const std::vector<Point>& ps,
                                     const std::vector<P>& segs,
                                     const Intersect& inter) {
        sweep_line.Reset(ps.size());
        this->ps = &ps;
        this->inter = &inter;
        segs_visited.resize(segs.size());
        std::fill(segs_visited.begin(), segs_visited.end(), false);

        // wtf. it sorts by x...
        auto should_swap = [&](Index i_0, Index i_1) {
            return ps[i_0].x > ps[i_1].x || (ps[i_0].x == ps[i_1].x && ps[i_0].y > ps[i_1].y);
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
            // some events may not get inside in case multiple intersection in one point > 2
            // 2 have right or left endpoint over there...
            auto left = new LeftEndpointEvent(*this, i);
            event_queue.insert(E_PTR{(Event*)left});
            event_queue.insert(E_PTR{(Event*)new RightEndpointEvent(*this, i)});
        }
        // algorithm itself
        auto it = event_queue.begin();
        decltype(it) it_next;
        while (it != event_queue.end()) {
            (**it).Handle();
            //sweep_line.Print(std::cout);
            it_next = std::next(it);
            // should not remove intersetion events because 
            // can find past events in the future
            if (!dynamic_cast<IntersectionEvent*>(it->get())) {
                event_queue.erase(it);    
            }
            it = it_next;
        }
        return intersections;
    }
};


}