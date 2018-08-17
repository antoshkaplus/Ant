#pragma once

#include "triangle.hpp"


namespace ant::geometry::triangle {
            

template<class Id, Id NoneValue> 
class AdjacentTriangles {

    using Array = std::array<Id, 2>;
    using Neighbors = std::unordered_map<Edge, Array>;

public:
    
    void Replace(const Edge& e, Id from, Id to) {
        auto& ni = neighbors_[e];
        assert(ni[0] == from || ni[1] == from);
        std::swap(ni[0] == from ? ni[0] : ni[1], to);
    }
    
    void Remove(const Edge& e) {
        neighbors_.erase(e);
    }
    
    void Insert(const Edge& e, Id i_0, Id i_1) {
        neighbors_[e] = {{i_0, i_1}};
    }
    
    void Insert(const Edge& e, Id i) {
        auto it = neighbors_.find(e);
        if (it == neighbors_.end()) {
            neighbors_[e] = {{ i, NoneValue }};
        } else {
            auto& n = it->second; 
            char ch = (n[0] == NoneValue) ? 0 : 1;
            n[ch] = i;
        }
    }
    
    Id another(const Edge& e, Id id) const {
        auto& ni = neighbors_.at(e);
        assert(ni[0] == id || ni[1] == id);
        return ni[0] == id ? ni[1] : ni[0];
    }
    
    const Array& operator[](const Edge& e) const {
        return neighbors_.at(e);
    }

    auto begin() const {
        return neighbors_.begin();
    }
    
    auto end() const {
        return neighbors_.end();
    }

private:
    Neighbors neighbors_;
    
};

using AdjacentTrianglesIndex = AdjacentTriangles<Index, -1>;

template<class Id, Id NoneValue>
inline std::ostream& operator<<(std::ostream& out, const AdjacentTriangles<Id, NoneValue>& adjTries) {
    for (auto& p : adjTries) {
        out << p.first << ", " << ", TrieId: " << p.second[0] << " " << p.second[1] << std::endl;
    }
    return out;
}


}
