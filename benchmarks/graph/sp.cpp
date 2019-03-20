
#include <benchmark/benchmark.h>

#include "ant/graph/graph_3.hpp"
#include "ant/graph/partition_sp.hpp"
#include "ant/graph/sp.hpp"

using namespace ant;
using namespace ant::graph;

// run benchmarks for Dijstra vs Dejkstra for sparse graphs
// see what's the difference

// should use different args

//DijkstraShortestPath const EdgedGraph& graph, const std::vector<Value>& edgeValues
//
//std::tuple<Value, bool> Compute(Index src, Index dst)




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




}

// each test case consists of parameters:
// number of edges, number of nodes,

BENCHMARK_TEMPLATE(SP_Queries, Dijkstra_SP<EdgedGraph<Index, Index>, Int>);
BENCHMARK_TEMPLATE(SP_Queries, Partition_SP<Int>);
