#pragma once


#include "ant/graph/graph_traits.hpp"
#include "bfs.hpp"


namespace ant::graph {

template<class Process, typename Graph, std::enable_if_t<
        is_vertex_descriptor_index_v<Graph> && is_edge_value_v<Graph>, int> = 0>
void BFS_Weighted_Prev(Graph& gr, typename Graph::VertexDescriptor vs, Process&& pr) {
    BFS_Weighted_Prev(gr, std::vector{vs}, pr);
}


// TODO implement checking weights
// change all dependencies too !
template<class Process, typename Graph, std::enable_if_t<
        is_vertex_descriptor_index_v<Graph> && is_edge_value_v<Graph>, int> = 0>
void BFS_Weighted_Prev(Graph& gr, std::vector<typename Graph::VertexDescriptor> vs, Process&& pr) {

    struct Item {
        typename Graph::VertexDescriptor from;
        typename Graph::Advance advance;
        typename Graph::EdgeValue value;

        Item(typename Graph::VertexDescriptor from, typename Graph::Advance advance, typename Graph::EdgeValue value)
                : from(from), advance(std::move(advance)), value(std::move(value)) {}

        bool operator<(const Item& item) const {
            return value > item.value;
        }
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
                q.emplace(v, w, w.edge().value());
            }
        }
    }

    while (!q.empty()) {
        auto item = q.top();
        q.pop();

        auto to = gr.vertex(item.advance.to().descriptor());
        auto from = gr.vertex(item.from);

        if constexpr (std::is_same_v < void, decltype(pr(to, item.advance)) >) {
            pr(from, item.advance);
        } else {
            auto flow = pr(from, item.advance);
            if (flow == BFS_Flow::Terminate) return;
            if (flow == BFS_Flow::Skip) continue;
            if (flow == BFS_Flow::Revisit) throw std::logic_error("operation not supported");
        }
        for (auto w : to.advance()) {
            auto w_to = w.to().descriptor();
            if (!visited[w_to]) {
                visited[w_to] = true;
                q.emplace(to.descriptor(), w, item.value + w.edge().value());
            }
        }
    }
}

}