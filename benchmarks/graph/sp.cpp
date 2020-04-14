#include <benchmark/benchmark.h>

#include "ant/graph/graph.hpp"
#include "ant/graph/algo/dijkstra.hpp"
#include "ant/graph/algo/dijkstra_components.hpp"

#include "test/graph/graph.hpp"

using namespace ant;
using namespace ant::graph;

using Policy = ant::graph::policy::Policy<ant::graph::policy::EdgeValue<Count>>;

struct TestCase {
    Count test_count;
    Count node_count;
    Count edge_count_extra;
    Int max_value;
    Count query_count;
};

vector<TestCase> kSmallTestCases_1 = {
    {1, 10'000, 0, 1000, 5'000}, {1, 10'000, 0, 1000, 5'000},
    {1, 10'000, 10, 1000, 5'000}, {1, 10'000, 10, 1000, 5'000},
    {1, 10'000, 25, 1000, 5'000}, {1, 10'000, 25, 1000, 5'000}};

const std::vector<TestCase> kTestCases_1 = {
    {1, 100'000, 0, 1000, 50'000}, {1, 100'000, 0, 10000, 50'000},
    {1, 100'000, 25, 1000, 50'000}, {1, 100'000, 25, 10000, 50'000},
    {1, 100'000, 50, 1000, 50'000}, {1, 100'000, 50, 10000, 50'000}
};


const std::vector<TestCase> kTestCases_10 = {
    {10, 100'000, 0, 1000, 50'000}, {10, 100'000, 0, 10000, 50'000},
    {10, 100'000, 25, 1000, 50'000}, {10, 100'000, 25, 10000, 50'000},
    {10, 100'000, 50, 1000, 50'000}, {10, 100'000, 50, 10000, 50'000}
};

auto k_Full_TestCases = kTestCases_1;
auto k_Precompute_TestCases = kTestCases_1;

template<typename RNG>
void Precompute(Dijkstra<Graph<Policy>>&, RNG&) {}

template<typename RNG>
void Precompute(DijkstraComponents<Graph<Policy>>& method, RNG& rng) {
    method.Precompute(rng);
}

template<typename RNG>
void Precompute(DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Linear>& method, RNG& rng) {
    auto cluster_count = std::max(1, static_cast<Count>(std::sqrt(CountVertices(method.graph()))));
    auto clustering = TrivialClustering_NoLoops(method.graph(), cluster_count, rng);
    method.Compute(clustering);
}

template<typename RNG>
void Precompute(DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Dijkstra>& method, RNG& rng) {
    auto cluster_count = std::max(1, static_cast<Count>(std::sqrt(CountVertices(method.graph()))));
    auto clustering = TrivialClustering_NoLoops(method.graph(), cluster_count, rng);
    method.Compute(clustering);
}

template <typename SP_Solver>
void SP_Queries(benchmark::State& state) {

    std::default_random_engine rng;
    auto test_case = k_Full_TestCases[state.range(0)];

    for (auto _ : state) {
        for (auto i = 0; i < test_case.test_count; ++i) {
            state.PauseTiming();

            auto graph = ant::graph::test::RandomConnectedEdgedValueGraph<Policy>(test_case.node_count,
                    test_case.node_count - 1 + test_case.edge_count_extra, test_case.max_value, rng);
            std::vector<std::array<Index, 2>> queries(test_case.query_count);

            std::uniform_int_distribution<> node_distr(0, test_case.node_count-1);
            for (auto& q : queries) {
                q[0] = node_distr(rng);
                q[1] = node_distr(rng);
            }

            state.ResumeTiming();

            SP_Solver solver(graph);
            Precompute(solver, rng);
            for (auto q : queries) {
                if constexpr (std::is_same_v<SP_Solver, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Linear>>
                    || std::is_same_v<SP_Solver, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Dijkstra>>) {
                    solver.Dist(q[0], q[1]);
                } else {
                    solver.Compute(q[0], q[1]);
                }
            }
        }
    }
}

template <typename SP_Solver>
void SP_Precompute(benchmark::State& state) {
    std::default_random_engine rng;
    auto test_case = k_Precompute_TestCases[state.range(0)];

    for (auto _ : state) {
        for (auto i = 0; i < test_case.test_count; ++i) {
            state.PauseTiming();

            auto graph = ant::graph::test::RandomConnectedEdgedValueGraph<Policy>(test_case.node_count,
                                                                                  test_case.node_count - 1 + test_case.edge_count_extra, test_case.max_value, rng);
            std::vector<std::array<Index, 2>> queries(test_case.query_count);

            std::uniform_int_distribution<> node_distr(0, test_case.node_count-1);
            for (auto& q : queries) {
                q[0] = node_distr(rng);
                q[1] = node_distr(rng);
            }

            state.ResumeTiming();

            SP_Solver solver(graph);
            Precompute(solver, rng);
        }
    }
}


BENCHMARK_TEMPLATE(SP_Queries, Dijkstra<Graph<Policy>>)->DenseRange(0, 5)->Iterations(1);
BENCHMARK_TEMPLATE(SP_Queries, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Dijkstra>)->DenseRange(0, 5)->Iterations(1);
BENCHMARK_TEMPLATE(SP_Queries, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Linear>)->DenseRange(0, 5)->Iterations(1);


BENCHMARK_TEMPLATE(SP_Precompute, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Dijkstra>)->DenseRange(0, 5)->Iterations(1);
BENCHMARK_TEMPLATE(SP_Precompute, DijkstraComponentsBase<Graph<Policy>, DijkstraComponentsBase_ComponentDistances::Linear>)->DenseRange(0, 5)->Iterations(1);