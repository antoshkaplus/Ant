
#include <benchmark/benchmark.h>

#include "ant/core/core.hpp"

using namespace std;
using namespace ant;

template<class T>
void VectorBoolPush(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<T> vs;
        for (auto i = 0; i < 100000; ++i) {
            vs.push_back(T());
        }
        benchmark::DoNotOptimize(vs.back());
    }
}

template<class T>
void VectorBoolTraverse(benchmark::State& state) {
    std::vector<T> vs(100000);
    for (auto _ : state) {
        for (auto v : vs) {
            benchmark::DoNotOptimize(v);
        }
    }
}

template<class T>
void VectorBoolRandomAccess(benchmark::State& state) {
    int size = 100000;

    std::vector<T> vs(size);

    std::vector<Index> indices;
    for (auto i = 0; i < size; ++i) {
        indices.resize(indices.size() + 10, i);
    }
    std::default_random_engine rng;
    std::shuffle(indices.begin(), indices.end(), rng);

    for (auto _ : state) {
        for (auto i : indices) {
            benchmark::DoNotOptimize(vs[i]);
        }
    }
}


BENCHMARK_TEMPLATE(VectorBoolPush, bool);
BENCHMARK_TEMPLATE(VectorBoolPush, char);
BENCHMARK_TEMPLATE(VectorBoolPush, int);
BENCHMARK_TEMPLATE(VectorBoolTraverse, bool);
BENCHMARK_TEMPLATE(VectorBoolTraverse, char);
BENCHMARK_TEMPLATE(VectorBoolTraverse, int);
BENCHMARK_TEMPLATE(VectorBoolRandomAccess, bool);
BENCHMARK_TEMPLATE(VectorBoolRandomAccess, char);
BENCHMARK_TEMPLATE(VectorBoolRandomAccess, int);