//
//  path.hpp
//  Ant
//
//  Created by Anton Logunov on 11/18/16.
//
//

#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/graph_3.hpp"
#include "ant/graph/bfs_3.hpp"


namespace ant {

namespace graph {


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
        dist_(from, to) = val;
        max_ += val;
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


template<class EdgedGraph, class Value>
class DijkstraShortestPath {
    
    struct Item {
        Index dst;
        Value val;
        
        Item(Index dst, Value val) 
            : dst(dst), val(val) {}
        
        bool operator<(const Item& t) const {
            // need min priority queue
            return val > t.val;
        }
    };

    struct Item2 {
        Index orig;
        Index dst;
        Value val;
        
        Item2(Index orig, Index dst, Value val)
            : orig(orig), dst(dst), val(val) {}

        bool operator<(const Item& t) const {
            // need min priority queue
            return val > t.val;
        }

    };
    
    struct R2 {
        Index orig;
        Value val;
    };
    

public:
    DijkstraShortestPath(const EdgedGraph& graph, const std::vector<Value>& edgeValues) 
        : graph(graph), edgeValues(edgeValues), vis(graph.nodeCount()), res(graph.nodeCount()) {
        
        vis2[0].resize(graph.nodeCount());
        vis2[1].resize(graph.nodeCount());
    }
    
    // for each how much to travel
    // could return ref, and keep array for reuse
    // but that way it can be moved to the client actually
    std::tuple<std::vector<Value>, std::vector<bool>> Compute(Index origin) {
        std::vector<bool> visited(graph.nodeCount(), false);
        std::vector<Value> res(graph.nodeCount(), std::numeric_limits<Value>::max());
        res[origin] = 0;
        // by distance put dest
        std::priority_queue<Item> q;
        q.emplace(origin, 0);
        while (!q.empty()) {
            Item t = q.top();
            q.pop();
            if (visited[t.dst]) continue;
            visited[t.dst] = true;
            for (auto p : graph.nextPairs(t.dst)) {
                auto v = t.val + edgeValues[p.edge];
                if (v < res[p.node]) {
                    res[p.node] = v;
                    q.emplace(p.node, v);
                }
            }
        }
        return std::make_tuple(res, visited);
    }
    
    
    std::tuple<Value, bool> Compute(Index src, Index dst) {
        std::fill(vis.begin(), vis.end(), false);
        std::priority_queue<Item> q;
        q.emplace(src, 0);
        while (!q.empty()) {
            auto t = q.top();
            q.pop();
            if (vis[t.dst]) continue;
            if (t.dst == dst) {
                return {t.val, true};
            }
            vis[t.dst] = true;
            for (auto p : graph.nextPairs(t.dst)) {
                if (!vis[p.node]) {
                    q.emplace(p.node, t.val + edgeValues[p.edge]);
                }
            }
        }
        return {0, false};
    }
    
    std::tuple<Value, bool> Compute2(Index src, Index dst) {
        std::fill(vis2[0].begin(), vis2[0].end(), false);
        std::fill(vis2[1].begin(), vis2[1].end(), false);
        
        // what if we use one priority queue?
        // how do we get connected?
        // Item should have child.. from where it came from
        // 
        std::priority_queue<Item> q[2];
        q[0].emplace(src, 0);
        q[1].emplace(dst, 0);
        
        for (;;) {
            auto i = q[0].size() < q[1].size() ? 0 : 1;
            auto t = q[i].top();
            q[i].pop();
            if (vis2[i][t.dst]) continue;
            if (vis2[!i][t.dst]) {
                return make_tuple(res[t.dst] + t.val, true);
            }
            res[t.dst] = t.val;
            vis2[i][t.dst] = true;
            
            for (auto p : graph.nextPairs(t.dst)) {
                if (!vis2[i][p.node]) {
                    q[i].emplace(p.node, t.val + edgeValues[p.edge]);
                }
            }
        }
        return {0, false};
    }
    

    std::tuple<Value, bool> Compute22(Index src, Index dst) {
        std::fill(rs.begin(), rs.end(), {-1, 0});
        
        // what if we use one priority queue?
        // how do we get connected?
        // Item should have child.. from where it came from
        // 
        std::priority_queue<Item> q;
        q.emplace(src, src, 0);
        q.emplace(dst, dst, 0);
        
        for (;;) {
            auto t = q.top();
            q.pop();
            if (rs[t.dst].orig == t.orig) continue;
            if (rs[t.dst].orig != -1) {
                // found it
                return std::make_tuple(rs[t.dst].val + t.val, true);
            }
            rs[t.dst] = {t.orig, t.val};
                        
            for (auto p : graph.nextPairs(t.dst)) {
                if (rs[p.node].orig != t.orig) {
                    q.emplace(t.orig, p.node, t.val + edgeValues[p.edge]);
                }
            }
        }
        return std::make_tuple(0, false);
    }

    
private:
    const EdgedGraph& graph;
    const std::vector<Value>& edgeValues;
    
    std::vector<R2> rs; 
    
    std::vector<bool> vis;
    
    std::vector<bool> vis2[2];
    
    std::vector<Value> res;
};


// give me centers and distances to them

// so result probably should be, cluster center + radius
template<class EdgedGraph, class Value>
struct CenterClustering {
        
    CenterClustering(const EdgedGraph& g, const std::vector<Value>& vals)
        : g(g), vals(vals) {}

    // each vertex belongs to certain cluster
    std::vector<Index> partition(Count k) {
        
        std::vector<Index> clusters(g.nodeCount());
        
        std::default_random_engine rng;
        std::uniform_int_distribution<> distr_v(0, g.nodeCount()-1);
        
        auto& origs = centers_;
        origs.clear();
        for (auto i = 0; i < k;) {
            auto n = distr_v(rng);
            auto it = lower_bound(origs.begin(), origs.end(), n);
            if (*it == n) {
                continue;
            }
            origs.insert(it, n);
            ++i;
        }
        
        return exec(origs);
    }
    
    // now we do same thing with oversampling
    // but for this one we need deletion strategy
    
    // how many clusters from 64 and use power of 2
    // how big oversampling: k(log k) 
    
    // n is oversampling
    
    // the strategy is for now by size
    std::vector<Index> partition(Count k, Count n) {
        struct P {
            Index idx;
            Count sz;
        };
        std::vector<P> ps(n, {0, 0});
        auto i = 0;
        for (auto& p : ps) p.idx = i++; 
        
        auto cs = partition(n);
        for (auto c : cs) {
            ++ps[c].sz;
        }
        
        std::partial_sort(ps.begin(), ps.end()-k, ps.end());
        std::vector<bool> res(ps.size(), true);
        for (auto i = 0; i < n-k; ++i) {
            res[ps[i].idx] = false;
        }
        DecreaseClustering(cs, res);
        auto it = std::remove_if(centers_.begin(), centers_.end(), [&](Index c){ return cs[c] == -1; });
        centers_.erase(it, centers_.end());
        
        auto redoCount = std::count(cs.begin(), cs.end(), -1);
        std::vector<Index> origs;
        origs.reserve(redoCount);
        for (int i = 0; i < cs.size(); ++i) {
            if (cs[i] != -1) origs.push_back(i);
        }
        
        return exec(origs);
    }
    
    const std::vector<Index>& centers() const {
        return centers_;
    }
    
    const std::vector<Value>& radius() const {
        return radius_;
    } 
    
private:

    std::vector<Index> exec(const std::vector<Index>& origs) {
        std::vector<Index> clusters(g.nodeCount());
        for (auto i = 0; i < origs.size(); ++i) {
            clusters[origs[i]] = i;
        }
        
        radius_.resize(centers_.size());
        std::fill(radius_.begin(), radius_.end(), 0);
        auto proc = [&](Index to, Index from, Index edge, Value val) {
            clusters[to] = clusters[from];
            
            if (val > radius_[clusters[from]]) {
                radius_[clusters[from]] = val;
            }
        };
        
        WeightedBFS<EdgedGraph, Value> bfs(g, vals); 
        bfs.runPrev(origs, proc);
        return clusters;

    }
    
    std::vector<Value> radius_;
    std::vector<Index> centers_;
 
    const EdgedGraph& g;
    const std::vector<Value>& vals;
};  

// Look for scientific paper:
// Goal Directed Shortest Path Queries Using 
// Precomputed Cluster Distances

// what if it's sparse graph and can be clustered in trees

// also please take a look at Highway Hierarchies
// and Component Hierarchies

// Highway hierarchies hasten exact shortest path queries
// Reach for A∗ : Efficient point-to-point shortest path algorithms

// An extremely fast, exact algorithm for finding shortest paths in static networks with geographical background
template<class EdgedGraph, class Value>
class ShortestPathLargeGraph {
    
    struct Item {
        Value val;
        
        Index row_border;
        Index col_border;
    };
    
public:
    ShortestPathLargeGraph(const EdgedGraph& g, const std::vector<Value>& vals) 
        : g(g), vals(vals) {
        
        // have to define k, n
        clusters = center_clustering.partition(1, 1);
        ComputeClusterPairs();
    }
    
    std::tuple<Value, bool> Compute(Index src, Index dst) {
        
        // so we have two queues 
        // two elements first at each one
        // we start doing stuff with them..
        // get elements out of them one after another
        // each time has to keep track of distances
            
        // when we come to new location we check if it's our guy,
        // if yes put in our hash table
        // and continue our search
        
        // the thing is it may still requere a lot.
        
        // implementation on demand later
                    
        // we came to new location what to do? 
        Index cur;
        ++cur;
    }
    
private:
    
    void ComputeClusterPairs() {
        WeightedBFS<EdgedGraph, Value> bfs(g, vals);
        
        std::vector<Index> which_orig(g.nodeCount());
        for (auto r = 0; r < cluster_count; ++r) {
            std::vector<Index> origs;
            for (auto i = 0; i < g.nodeCount(); ++i) {
                if (clusters[i] == r){
                  origs.push_back(i);
                  which_orig[i] = i;  
                } 
            }
            
            for (auto c = r+1; c < cluster_count; ++c) {
                Index r_start;
                Index c_end;
                Value rc_val;
                auto pr = [&](Index to, Index from, Index edge, Value val) {
                    which_orig[to] = which_orig[from];
                    if (clusters[to] == c) {
                        c_end = to;
                        r_start = which_orig[c_end];
                        rc_val = val;
                        return BFS_Flow::Terminate;
                    }
                    return BFS_Flow::Continue;
                };
                bfs.runPrev(origs, pr);
                
                cluster_pairs(r, c) = {r_start, c_end, rc_val};
                cluster_pairs(c, r) = {c_end, r_start, rc_val};
            }
        }
    }
    
    std::unordered_map<Index, Value> borderDist;
    CenterClustering<EdgedGraph, Value> center_clustering;
    Count cluster_count;
    std::vector<Index> clusters;
    grid::Grid<Item> cluster_pairs;
    
    const EdgedGraph& g;
    const std::vector<Value>& vals;
};


}

}

