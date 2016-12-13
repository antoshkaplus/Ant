
#include <iostream>
#include <list>


#include "ant/core/core.hpp"



namespace ant { 

namespace geometry {

namespace d1 {



struct Segment {
    Segment() {}
    Segment(Int origin, Int length) : origin(origin), length(length) {}
    
    
    Segment united(const Segment& s) const {
        Segment r;
        r.origin = std::min(first(), s.first()); 
        r.length = std::max(last(), s.last()) - r.origin;
        return r;
    }
    
    bool isIntersect(const Segment& s) const {
        return !(origin >= s.origin + s.length || s.origin >= origin + length); 
    }
    
    bool isInside(const Segment& s) const {
        return origin <= s.origin && origin + length >= s.origin + length;
    }
    
    bool isInside(Int p) const {
        return p >= origin && p <= origin + length;
    }
    
    void set_first(Int first) {
        length += origin - first;
        origin = first;
    }
    
    void set_last(Int last) {
        length = last - origin;
    }
    
    Int first() const {
        return origin;
    }
    
    Int last() const {
        return origin + length;
    }
    
    bool isEmpty() const {
        return length <= 0;
    }
    
    
    static Segment united(const Segment& s_1, const Segment& s_2) {
        auto origin = std::min(s_1.origin, s_2.origin);
        auto length = std::max(s_1.last(), s_2.last()) - origin;
        return {origin, length};
    }
    
    Int origin, length;
};

bool operator==(const Segment& s_0, const Segment& s_1);

template<class V> 
struct ValuedSegmentList;

template<class V>
std::ostream& operator<<(std::ostream& output, const ValuedSegmentList<V>& vs);

template<class V> 
struct ValuedSegmentList {
    struct Pair {
        Pair() {}
        Pair(const Segment& segment, const V& value) 
        : segment(segment), value(value) {}
        Segment segment;
        V value;
    };
    
    Pair& front() {
        return list_.front();
    }
    const Pair& front() const {
        return list_.front();
    }
    
    Pair& back() {
        return list_.back();
    }
    const Pair& back() const {
        return list_.back();
    }
    
    using const_iterator = typename std::list<Pair>::const_iterator;
    
    const_iterator begin() const {
        return list_.begin();
    }
    const_iterator end() const {
        return list_.end();
    }
    
    bool isInside(const Segment& s) {
        auto a = s.origin;
        Count found = 0;
        auto it = begin();
        while (it != end() && found != 2) {
            if (it->segment.isInside(a)) {
                a = s.origin + s.length;
                ++found;
            }
            else ++it;
        }
        return found == 2;
    }
    
    bool isContinious() const {
        bool result = true;
        const_iterator it_prev, it = begin();
        if (it == end()) goto finish;
        while (true) {
            it_prev = it;
            ++(it = it_prev);
            if (it == end()) break;
            if (it_prev->segment.last() != it->segment.first()) {
                result = false;
                break;                
            }
        }
    finish:
        return result;
    }
    
    V max_value(const Segment& s) {
        auto it = list_.begin();
        while (it != list_.end() && !it->segment.isIntersect(s)) ++it;
        assert(it != list_.end()); 
        V v = (it++)->value;
        while (it != list_.end() && it->segment.isIntersect(s)) {
            if (it->value > v) v = it->value;
            ++it;
        }
        return v; 
    }
    
    void insert(const Segment& segment, const V& value) {
        auto b = list_.begin();
        while (b != list_.end() && b->segment.origin < segment.origin) {
            ++b;
        }
        b = list_.emplace(b, segment, value);
        // handling first previous element
        if (b != list_.begin()) {
            auto prev = b; --prev;
            if (prev->segment.isIntersect(b->segment)) {
                if (prev->value > b->value) {
                    b->segment.set_first(prev->segment.last());
                    if (b->segment.isEmpty()) {
                        list_.erase(b);
                        // now 'b' is illegal so should return now
                        return;
                    }
                }
                else if (prev->value == b->value) {
                    b->segment.set_first(std::min(prev->segment.first(), b->segment.first()));
                    b->segment.set_last(std::max(prev->segment.last(), b->segment.last()));
                    list_.erase(prev); 
                }
                else {
                    // maybe b inside prev
                    if (prev->segment.last() > b->segment.last()) {
                        Pair p{{b->segment.last(), prev->segment.last()-b->segment.last()}, prev->value};
                        // should insert after b
                        auto next = b; ++next;
                        list_.insert(next, p);
                    }
                    
                    prev->segment.set_last(b->segment.first());
                    if (prev->segment.isEmpty()) {
                        list_.erase(prev);
                    }
                }
            } else if (b->segment.first() == prev->segment.last() && prev->value == b->value) {
                b->segment.set_first(prev->segment.first());
                list_.erase(prev);
            }
        }
        // hangling a bunch of next elements
        while (true) {
            // declare for symmetricity
            auto next = b; ++next;
            if (next == list_.end()) break;
            if (!next->segment.isIntersect(b->segment)) {
                if (next->segment.first() == b->segment.last() && next->value == b->value) {
                    b->segment.set_last(next->segment.last());
                    list_.erase(next);
                }
                break;   
            }
            if (next->value > b->value) {
                // left side
                b->segment.set_last(next->segment.origin);
                if (b->segment.isEmpty()) {
                    list_.erase(b);
                    //can't return yet
                }
                // right side
                Segment s = segment; 
                s.set_first(next->segment.last());
                if (s.isEmpty()) {
                    // nothing to handle from this point
                    return; 
                }
                b = next; ++b;
                b = list_.emplace(b, s, value);
            }
            else if (next->value == b->value) {
                b->segment.set_last(std::max(b->segment.last(), next->segment.last()));
                list_.erase(next);
            }
            else {
                next->segment.set_first(b->segment.last());
                if (next->segment.isEmpty()) list_.erase(next);
            }
        }
    }
    
    // probably should call more low level method
    void insert(const Pair& pair) {
        insert(pair.segment, pair.value);
    }
private:
    std::list<Pair> list_;
    
    friend std::ostream& operator<< <V>(std::ostream& output, const ValuedSegmentList<V>& vs);
};

template<class V>
std::ostream& operator<<(std::ostream& output, const ValuedSegmentList<V>& vs) {
    for (auto& p : vs.list_) {
        output << p.segment.first() << ", " << p.segment.last() << " with value: " << p.value << std::endl;
    }
    return output;
}



} // namespace d1

} // namespace geometry

} // namespace ant
