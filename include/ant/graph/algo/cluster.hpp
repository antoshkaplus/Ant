#pragma once


#include "bfs_weighted.hpp"


namespace ant::graph {

// it's often can be a case where cluster is an argument
// therefore we have to keep cluster in it's own data structure
struct Clustering {
    std::vector<Index> vertex_cluster;
    Count cluster_count;
};

template<class Graph>
class CenterClustering {

    using Value = typename Graph::EdgeValue;

    std::vector<Value> radius_;
    std::vector<Index> centers_;

    Graph& g;

public:
    CenterClustering(Graph& g) : g(g) {}

    // TODO FIX THIS CODE have no idea what's going on here

    // returns array of vertex center
    template<class Rng>
    std::vector<Index> GenerateClusters(Count k, Rng& rng) {
        std::vector<Index> clusters(g.nodeCount());

        std::uniform_int_distribution<> distr_v(0, g.nodeCount()-1);

        auto& origs = centers_;
        origs.clear();
        for (auto i = 0; i < k;) {
            auto n = distr_v(rng);
            auto it = lower_bound(origs.begin(), origs.end(), n);
            if (*it == n) {
                continue;
            }
            // vector...
            origs.insert(it, n);
            ++i;
        }

        return GenerateClusters(origs);
    }

    // now we do same thing with oversampling
    // but for this one we need deletion strategy

    // how many clusters from 64 and use power of 2
    // how big oversampling: k(log k)

    // n is oversampling

    // the strategy is for now by size
    template<class Rng>
    std::vector<Index> partition(Count k, Count n, Rng& rng) {
        struct P {
            Index idx;
            Count sz;

            bool operator<(const P& p) const {
                return sz < p.sz;
            }
        };
        std::vector<P> ps(n, {0, 0});
        auto i = 0;
        for (auto& p : ps) p.idx = i++;

        auto cs = GenerateClusters(n, rng);
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

        return GenerateClusters(origs);
    }

    const std::vector<Index>& centers() const {
        return centers_;
    }

    const std::vector<Value>& radius() const {
        return radius_;
    }

private:

    std::vector<Index> GenerateClusters(const std::vector<Index>& origs) {
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

        BFS_Weighted_Prev(g, origs, proc);
        return clusters;

    }
};

}