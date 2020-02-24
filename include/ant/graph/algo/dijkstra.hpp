#pragma once

#pragma once


#include "ant/grid/grid.hpp"
#include "ant/graph/model/graph_3.hpp"
#include "ant/graph/bfs_3.hpp"
#include "ant/graph/cluster.hpp"


namespace ant::graph {

template<class EdgedGraph, class Value>
class Dijkstra {

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
    Dijkstra(const EdgedGraph& graph, const std::vector<Value>& edgeValues)
            : graph(graph), edgeValues(edgeValues), vis(graph.nodeCount()), res(graph.nodeCount()), rs(graph.nodeCount()) {

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

            for (auto p : graph.nextPairs(t.dst)) {
                if (rs[p.node].orig != t.orig) {
                    q.emplace(t.orig, p.node, t.val + edgeValues[p.edge]);
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
    const EdgedGraph& graph;
    const std::vector<Value>& edgeValues;

    std::vector<R2> rs;

    std::vector<bool> vis;

    std::vector<bool> vis2[2];

    std::vector<Value> res;
};

}
