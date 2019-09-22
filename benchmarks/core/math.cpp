

#include <ant/core/skip_list/skip_list_set.hpp>
#include "benchmark/benchmark.h"

#include "ant/core/math.hpp"

template<class Log2>
void BM_Log2(benchmark::State& state, Log2&& log2) {
    auto iterations = 10000UL;

    std::default_random_engine rng;
    std::uniform_int_distribution<uint64_t> distr;

    std::vector<uint64_t> numbers(iterations);
    for (auto& n : numbers) {
        n = distr(rng);
    }

    for (auto _ : state) {
        for (auto n : numbers) {
            benchmark::DoNotOptimize(log2(n));
        }
    }
}

int32_t log2_std(uint64_t number) {
    return std::log2(number);
}

typedef int32_t(*Func)(uint64_t);

BENCHMARK_CAPTURE(BM_Log2, STD, Func(log2_std));
BENCHMARK_CAPTURE(BM_Log2, ANT, Func(ant::log2));
