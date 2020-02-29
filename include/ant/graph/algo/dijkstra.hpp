#pragma once


#include "ant/grid/grid.hpp"
#include "cluster.hpp"


namespace ant::graph {

template<typename Graph>
class Dijkstra {

    using Value = typename Graph::EdgeValue;

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
    Dijkstra(Graph& graph)
            : graph(graph), vis(CountVertices(graph)), res(CountVertices(graph)), rs(CountVertices(graph)) {

        vis2[0].resize(CountVertices(graph));
        vis2[1].resize(CountVertices(graph));
    }

    // for each how much to travel
    // could return ref, and keep array for reuse
    // but that way it can be moved to the client actually
    std::tuple<std::vector<Value>, std::vector<bool>> Compute(Index origin) {
        std::vector<bool> visited(CountVertices(graph), false);
        std::vector<Value> res(CountVertices(graph), std::numeric_limits<Value>::max());
        res[origin] = 0;
        // by distance put dest
        std::priority_queue<Item> q;
        q.emplace(origin, 0);
        while (!q.empty()) {
            Item t = q.top();
            q.pop();
            if (visited[t.dst]) continue;
            visited[t.dst] = true;
            for (auto p : graph.vertex(t.dst).advance()) {
                auto v = t.val + p.edge().value();
                if (v < res[p.to().descriptor()]) {
                    res[p.to().descriptor()] = v;
                    q.emplace(p.to().descriptor(), v);
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
            for (auto p : graph.vertex(t.dst).advance()) {
                if (!vis[p.to().descriptor()]) {
                    q.emplace(p.to().descriptor(), t.val + p.edge().value());
                }
            }
        }
        return {0, false};
    }

    std::tuple<Value, bool> Compute22(Index src, Index dst) {
        std::fill(rs.begin(), rs.end(), R2{-1, 0});

        struct Item {
            Index orig;
            Index dst;
            Value val;

            Item() {}

            Item(Index orig, Index dst, Value val) : orig(orig), dst(dst), val(val) {}

            bool operator<(const Item& item) const {
                return val > item.val;
            }
        };

        // what if we use one priority queue?
        // how do we get connected?
        // Item should have child.. from where it came from
        //
        std::priority_queue<Item> q;
        q.emplace(src, src, 0);
        q.emplace(dst, dst, 0);

        Value value = 0;
        for (;;) {
            if (q.empty()) break;
            auto t = q.top();
            q.pop();
            if (rs[t.dst].orig == t.orig) continue;
            if (rs[t.dst].orig != -1) {
                value = rs[t.dst].val + t.val;
                break;
            }
            rs[t.dst] = {t.orig, t.val};

            for (auto p : graph.vertex(t.dst).advance()) {
                if (rs[p.to().descriptor()].orig != t.orig) {
                    q.emplace(t.orig, p.to().descriptor(), t.val + p.edge().value());
                }
            }
        }
        if (value == 0) {
            return std::make_tuple(0, false);
        }
        while (!q.empty()) {
            auto t = q.top();
            q.pop();
            if (rs[t.dst].orig == t.orig) continue;
            if (rs[t.dst].orig != -1 && rs[t.dst].val + t.val < value) {
                value = rs[t.dst].val + t.val;
            }
        }
        return std::make_tuple(value, true);
    }


private:
    Graph& graph;

    std::vector<R2> rs;

    std::vector<bool> vis;

    std::vector<bool> vis2[2];

    std::vector<Value> res;
};

}
