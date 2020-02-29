
#include "gtest/gtest.h"
#include "ant/graph/model/adj_vec/graph.hpp"
#include "ant/graph/algo/bfs.hpp"
#include "ant/graph/policies.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::model::adj_vec;

TEST(Graph_Algo_Bfs, Undirected) {
    Graph<ant::graph::policy::BasePolicy> g;

    decltype(g)::Mutator mutator(g);

    mutator.SetVertexCount(8);
    std::vector<std::pair<Index, Index>> edges = {
        {0, 1}, {1, 2}, {0, 2}, {2, 3}, {3, 4},
        {5, 6}, {6, 7}, {5, 7}
    };
    for (auto e : edges) {
        mutator.AddEdge(e.first, e.second);
    }

    std::vector<char> visited(8, false);
    graph::BFS(g, 0, [&](auto v) {
        visited[v.descriptor()] = true;
    });
    ASSERT_TRUE(all_of(visited.begin(), visited.begin() + 5, Identity_ConstRef));
    ASSERT_FALSE(any_of(visited.begin() + 5, visited.end(), Identity_ConstRef));
}

TEST(Graph_Algo_Bfs, Directed) {
    Graph<ant::graph::policy::Policy<ant::graph::policy::Directed>> g;

    decltype(g)::Mutator mutator(g);

    mutator.SetVertexCount(6);
    std::vector<std::pair<Index, Index>> edges = {
            {0, 1}, {0, 2}, {1, 3}, {4, 1},
            {4, 5}, {2, 5}
    };
    for (auto e : edges) {
        mutator.AddEdge(e.first, e.second);
    }

    std::vector<char> visited(6, false);
    graph::BFS(g, 0, [&](auto v) {
        visited[v.descriptor()] = true;
    });

    std::vector expected_visited = {0, 1, 2, 3, 5};
    ASSERT_TRUE(all_of(expected_visited.begin(), expected_visited.end(), [&](auto i) { return visited[i]; }));
    ASSERT_FALSE(visited[4]);
}

TEST(Graph_Algo_BfsPrev, Directed) {
    Graph<ant::graph::policy::Policy<ant::graph::policy::Directed>> g;

    decltype(g)::Mutator mutator(g);

    mutator.SetVertexCount(6);
    std::vector<std::pair<Index, Index>> edges = {
            {0, 1}, {0, 2}, {1, 3}, {4, 1},
            {4, 5}, {2, 5}
    };
    for (auto e : edges) {
        mutator.AddEdge(e.first, e.second);
    }

    std::vector<char> from_visited(6, false);
    std::vector<char> to_visited(6, false);
    graph::BFS_Prev(g, 0, [&](auto from, auto advance) {
        from_visited[from.descriptor()] = true;
        to_visited[advance.to().descriptor()] = true;
        (void)advance.edge();
    });

    std::vector expected_from_visited = {0, 1, 2};
    std::vector expected_to_visited = {1, 2, 3, 5};
    ASSERT_TRUE(all_of(expected_from_visited.begin(), expected_from_visited.end(), [&](auto i) { return from_visited[i]; }));
    ASSERT_TRUE(all_of(expected_to_visited.begin(), expected_to_visited.end(), [&](auto i) { return to_visited[i]; }));
    ASSERT_FALSE(expected_from_visited[4] || expected_to_visited[4]);
}

}