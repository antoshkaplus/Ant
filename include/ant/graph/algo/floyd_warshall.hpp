#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/algo/cluster.hpp"


namespace ant::graph {

// finds shortest distances between all pairs of nodes in O(N^3) time
template<class Value>
class FloydWarshall {

    grid::Grid<Value> dist_;
    Value max_;

    int node_count() const {
        return dist_.row_count();
    }

    const static Value LIMIT;

public:

    FloydWarshall(int node_count)
            : dist_(node_count, node_count, LIMIT), max_(0) {}


    void AddDirectedDist(int from, int to, Value val) {
        if (dist_(from, to) > val) {
            dist_(from, to) = val;
            max_ += val;
        }
    }

    void Compute() {
        max_ += 1;
        for (auto& d : dist_) {
            if (d == LIMIT) d = max_;
        }
        auto V = node_count();
        for (auto i = 0; i < V; ++i) {
            dist_(i, i) = 0;
        }
        /*
         Add all vertices one by one to the set of intermediate vertices.
         ---> Before start of a iteration, we have shortest distances between all
         pairs of vertices such that the shortest distances consider only the
         vertices in set {0, 1, 2, .. k-1} as intermediate vertices.
         ----> After the end of a iteration, vertex no. k is added to the set of
         intermediate vertices and the set becomes {0, 1, 2, .. k}
         */
        for (auto k = 0; k < V; k++) {
            // Pick all vertices as source one by one
            for (auto i = 0; i < V; i++) {
                // Pick all vertices as destination for the
                // above picked source
                for (auto j = 0; j < V; j++) {
                    // If vertex k is on the shortest path from
                    // i to j, then update the value of dist[i][j]
                    if (dist_(i, k) + dist_(k, j) < dist_(i, j)) {
                        dist_(i, j) = dist_(i, k) + dist_(k, j);
                    }
                }
            }
        }
    }

    bool Exists(int from, int to) const {
        return dist_(from, to) != max_;
    }

    Value Dist(int from, int to) const {
        return dist_(from, to);
    }
};

template <class Value>
const Value FloydWarshall<Value>::LIMIT = std::numeric_limits<Value>::max();

}