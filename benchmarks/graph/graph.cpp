
#include <benchmark/benchmark.h>

#include "ant/graph/graph.hpp"
#include "ant/graph/dense_graph.hpp"
#include "ant/graph/graph_util.hpp"

using namespace ant;

template<class Builder>
void ForEachEdge(benchmark::State& state) {

    auto graph = graph::BuildRandom<Builder>(state.range(0), 0.1*state.range(1));
    for (auto _ : state) {
        auto edge_count = 0;
        graph::ForEachEdge(graph, [&](auto from, auto to) {
            edge_count += from+to;
        });
        benchmark::DoNotOptimize(edge_count);
    }
}

BENCHMARK_TEMPLATE(ForEachEdge, graph::GraphBuilder<true, int16_t>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachEdge, graph::GraphBuilder<false, int16_t>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachEdge, graph::DenseGraphBuilder<true>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachEdge, graph::DenseGraphBuilder<false>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});


template<class Builder>
void ForEachNextNode(benchmark::State& state) {

    auto graph = graph::BuildRandom<Builder>(state.range(0), 0.1*state.range(1));

    std::vector<Index> indices(graph.nodeCount());
    for (auto i = 0; i < graph.nodeCount(); ++i) {
        indices.resize(indices.size() + 10, i);
    }
    std::default_random_engine rng;
    std::shuffle(indices.begin(), indices.end(), rng);
    for (auto _ : state) {
        auto total = 0;
        for (auto i : indices) {
            graph.forEachNextNode(i, [&](auto j) {
                total += j;
            });
        }
        benchmark::DoNotOptimize(total);
    }
}

BENCHMARK_TEMPLATE(ForEachNextNode, graph::GraphBuilder<true, int16_t>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachNextNode, graph::GraphBuilder<false, int16_t>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachNextNode, graph::DenseGraphBuilder<true>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});
BENCHMARK_TEMPLATE(ForEachNextNode, graph::DenseGraphBuilder<false>)->Args({100, 5})->Args({100, 7})->Args({100, 9})
        ->Args({1000, 5})->Args({1000, 7})->Args({1000, 9});