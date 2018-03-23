//
// dereferencing shared_ptr and naked takes same time.
// the test shows better result for naked ptr.
// it's due to sequencial allocation and shared_ptr allocates more memory
// to store it's values underneath.
//

#include <benchmark/benchmark.h>

#include "ant/core/core.hpp"

using namespace std;
using namespace ant;

void SharedPtr(benchmark::State& state) {
    const int size = state.range(0);
    vector<shared_ptr<int>> vs(size);
    for (auto& v : vs) v.reset(new int(1));

    auto total = 0;
    for (auto _ : state) {
        for (auto& v : vs) {
            total += *v;
        }
    }
    benchmark::DoNotOptimize(total);
}

void NakedPtr(benchmark::State& state) {
    const int size = 2*state.range(0);
    vector<int*> vs(size);
    for (auto& v : vs) v = new int(1);

    auto total = 0;
    for (auto _ : state) {
        for (auto& v : vs) {
            total += *v;
        }
    }

    for (auto v : vs) delete v;
    benchmark::DoNotOptimize(total);
}

BENCHMARK(NakedPtr)->Arg(100000);
BENCHMARK(SharedPtr)->Arg(100000);
