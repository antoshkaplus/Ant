#pragma once

#include "util.hpp"

namespace ant::graph {

enum class BFS_Flow {
    Continue,
    Revisit,
    Skip, // don't expand on children
    Terminate
};

template<class Process, typename Graph, typename = std::enable_if_t<
        is_vertex_descriptor_index_v<Graph>>
void BFS(const Graph& gr, std::vector<Graph::VertexDescriptor> vs, Process& pr) {
    std::queue<Graph::VertexDescriptor> q;
    Count c = CountVertices(gr);
    std::vector<bool> visited(c, false);
    for (auto v : vs) {
        visited[v] = true;
        q.push(v);
    }
    while (!q.empty()) {
        auto v = gr.vertex(q.front());
        q.pop();

        if constexpr (std::is_same_v<void, decltype(pr(v))>) {
            pr(v);
        } else {
            auto flow = pr(v);
            if (flow == BFS_Flow::Terminate) return;
            if (flow == BFS_Flow::Skip) continue;
            if (flow == BFS_Flow::Revisit) {
                q.push(v.descriptor());
                continue;
            }
        }
        for (auto w : gr.vertex(v).advance()) {
            auto w_to = w.to().descriptor();
            if (!visited[w_to]) {
                visited[w_to] = true;
                q.push(w_to);
            }
        }
    }
}

template<class Process, typename Graph, typename = std::enable_if_t<
        is_vertex_descriptor_index_v<Graph>>
void BFS(const Graph& gr, Graph::VertexDescriptor vs, Process& pr) {
    BFS(gr, {vs}, pr);
}


template<class Process, typename Graph, typename = std::enable_if_t<
        is_vertex_descriptor_index_v<Graph>>
void BFS_Prev(const Graph& gr, std::vector<Graph::VertexDescriptor> vs, Process& pr) {
    struct Item {
        Graph::VertexDescriptor to;
        Graph::VertexDescriptor from;
    };

    std::queue<Item> q;
    Count c = CountVertices(gr);
    std::vector<bool> visited(c, false);
    for (auto v : vs) {
        visited[v] = true;
        q.emplace(v, v);
    }

    while (!q.empty()) {
        auto item = q.front();
        q.pop();

        auto to = gr.vertex(item.to);
        auto from = gr.vertex(item.from);

        if constexpr (std::is_same_v<void, decltype(pr(to, from))>) {
            pr(to, from);
        } else {
            auto flow = pr(to, from);
            if (flow == BFS_Flow::Terminate) return;
            if (flow == BFS_Flow::Skip) continue;
            if (flow == BFS_Flow::Revisit) {
                q.push(item);
                continue;
            }
        }
        for (auto w : to.advance()) {
            auto w_to = w.to().descriptor();
            if (!visited[w_to]) {
                visited[w_to] = true;
                q.emplace(w_to, item.to);
            }
        }
    }
}

template<class Process, typename Graph, typename = std::enable_if_t<
is_vertex_descriptor_index_v<Graph>>
void BFS_Prev(const Graph& gr, Graph::VertexDescriptor vs, Process& pr) {
    BFS_Prev(gr, {vs}, pr);
}


// RIGHT NOW WORKS FOR TREES
// can know the whole history by using our vector
template<typename Graph, typename = std::enable_if_t<
    is_vertex_descriptor_index_v<Graph>>
int Diameter(const Graph& gr) {
    Graph::VertexDescriptor last = 0;
    std::vector<Graph::VertexDescriptor> arr(CountVertices(gr));
    auto func = [&] (auto v, auto prev) {
        arr[v.descriptor()] = prev.descriptor();
        last = v.descriptor();
        return BFS_Flow::Continue;
    };
    BFS_Prev(gr, 0, func);
    arr[last] = last;
    BFS_Prev(gr, last, func);

    // now compute size
    Graph::VertexDescriptor cur = last;
    Count length = 0;
    while (arr[cur] != cur) {
        ++length;
        cur = arr[cur];
    }
    return length;
}

}