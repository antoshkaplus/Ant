#include <benchmark/benchmark.h>

#include <ant/core/core.hpp>
#include <ant/simd/reverse.hpp>

using namespace ant;

template <class TValue>
static void Simd_Reverse_Std(benchmark::State& state) {
    std::vector<TValue> vs(state.range(0));
    for (auto _ : state) {
        std::reverse(vs.begin(), vs.end());
    }
    benchmark::DoNotOptimize(vs[0]);
}

template <class TValue>
static void Simd_Reverse_Simd(benchmark::State& state) {
    std::vector<TValue> vs(state.range(0));
    for (auto _ : state) {
        ant::simd::Reverse(vs.data(), vs.size());
    }
    benchmark::DoNotOptimize(vs[0]);
}

BENCHMARK_TEMPLATE(Simd_Reverse_Std, uint8_t)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(Simd_Reverse_Simd, uint8_t)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(Simd_Reverse_Std, uint16_t)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(Simd_Reverse_Simd, uint16_t)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(Simd_Reverse_Std, uint32_t)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(Simd_Reverse_Simd, uint32_t)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(Simd_Reverse_Std, uint64_t)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(Simd_Reverse_Simd, uint64_t)->Arg(10000)->Arg(100000)->Arg(1000000);
