#pragma once


#include "ant/graph/graph_traits.hpp"
#include "bfs.hpp"


namespace ant::graph {

template<class Process, typename Graph, std::enable_if_t<
        is_vertex_descriptor_index_v<Graph> && is_edge_value_v<Graph>, int> = 0>
void BFS_Weighted_Prev(Graph& gr, typename Graph::VertexDescriptor vs, Process&& pr) {
    BFS_Weighted_Prev(gr, std::vector{vs}, pr);
}


template<class Process, typename Graph, std::enable_if_t<
        is_vertex_descriptor_index_v<Graph> && is_edge_value_v<Graph>, int> = 0>
void BFS_Weighted_Prev(Graph& gr, std::vector<typename Graph::VertexDescriptor> vs, Process&& pr) {

    struct Item {
        typename Graph::VertexDescriptor from;
        typename Graph::Advance advance;

        Item(typename Graph::VertexDescriptor from, typename Graph::Advance advance)
                : from(from), advance(std::move(advance)) {}
    };

    std::priority_queue<Item> q;
    Count c = CountVertices(gr);
    std::vector<bool> visited(c, false);
    for (auto v : vs) {
        visited[v] = true;
    }
    for (auto v : vs) {
        auto from = gr.vertex(v);
        for (auto w : from.advance()) {
            auto w_to = w.to().descriptor();
            if (!visited[w_to]) {
                visited[w_to] = true;
                q.emplace(v, w);
            }
        }
    }

    while (!q.empty()) {
        auto item = q.front();
        q.pop();

        auto to = gr.vertex(item.advance.to().descriptor());
        auto from = gr.vertex(item.from);

        if constexpr (std::is_same_v < void, decltype(pr(to, item.advance)) >) {
            pr(from, item.advance);
        } else {
            auto flow = pr(to, from);
            if (flow == BFS_Flow::Terminate) return;
            if (flow == BFS_Flow::Skip) continue;
            if (flow == BFS_Flow::Revisit) throw std::logic_error("operation not supported");
        }
        for (auto w : to.advance()) {
            auto w_to = w.to().descriptor();
            if (!visited[w_to]) {
                visited[w_to] = true;
                q.emplace(to.descriptor(), w);
            }
        }
    }
}

}