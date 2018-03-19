
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
        Count total = 0;
        for (auto v : vs) {
            total += v;
        }
        benchmark::DoNotOptimize(total);
    }
}

BENCHMARK_TEMPLATE(VectorBoolPush, bool);
BENCHMARK_TEMPLATE(VectorBoolPush, char);
BENCHMARK_TEMPLATE(VectorBoolPush, int);
BENCHMARK_TEMPLATE(VectorBoolTraverse, bool);
BENCHMARK_TEMPLATE(VectorBoolTraverse, char);
BENCHMARK_TEMPLATE(VectorBoolTraverse, int);