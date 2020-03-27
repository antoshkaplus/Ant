
#include <benchmark/benchmark.h>

#include "ant/graph/graph.hpp"
#include "ant/graph/partition_sp.hpp"
#include "ant/graph/sp.hpp"

#include "test/graph/graph.hpp"

using namespace ant;
using namespace ant::graph;

//DijkstraShortestPath const EdgedGraph& graph, const std::vector<Value>& edgeValues
//
//std::tuple<Value, bool> Compute(Index src, Index dst)


struct TestCase {
    Count test_count;
    Count node_count;
    Count edge_count_extra;
    Int max_value;
    Count query_count;
};

const std::vector<TestCase> kTestCases = {{10, 100'000, 0, 1000, 50'000}, {10, 100'000, 0, 10000, 50'000},
                                          {10, 100'000, 25, 1000, 50'000}, {10, 100'000, 25, 10000, 50'000},
                                          {10, 100'000, 50, 1000, 50'000}, {10, 100'000, 50, 10000, 50'000}};

template <typename Solver>
Int Query(Solver& solver, Index i_1, Index i_2);

template <>
Int Query(Partition_SP<Int>& solver, Index i_1, Index i_2) {
    return *solver.Query(i_1, i_2);
}

template <>
Int Query(Dijkstra_SP<EdgedGraph<Index, Index>, Int>& solver, Index i_1, Index i_2) {
    return std::get<0>(solver.Compute(i_1, i_2));
}

template <typename SP_Solver>
void SP_Queries(benchmark::State& state) {
    std::default_random_engine rng;
    auto test_case = kTestCases[state.range(0)];

    for (auto _ : state) {
        for (auto i = 0; i < test_case.test_count; ++i) {
            state.PauseTiming();

            // need a policy to go with it
            auto graph_weights = test::RandomEdgeValueGraph(test_case.node_count,
                    test_case.node_count - 1 + test_case.edge_count_extra, test_case.max_value, rng);
            std::vector<std::array<Index, 2>> queries(test_case.query_count);

            std::uniform_int_distribution<> node_distr(0, test_case.node_count-1);
            for (auto& q : queries) {
                q[0] = node_distr(rng);
                q[1] = node_distr(rng);
            }

            state.ResumeTiming();

            SP_Solver solver(graph_weights.graph, graph_weights.edge_weights);
            for (auto q : queries) {
                Query(solver, q[0], q[1]);
            }
        }
    }
}


BENCHMARK_TEMPLATE(SP_Queries, Dijkstra_SP<EdgedGraph<Index, Index>, Int>)->DenseRange(0, 5)->Iterations(1);
BENCHMARK_TEMPLATE(SP_Queries, Partition_SP<Int>)->DenseRange(0, 5)->Iterations(1);
