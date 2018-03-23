
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



void UnorderedMap_1(benchmark::State& state) {
    const int size = state.range(0);
    const double ratio = 0.1 * state.range(1);
    vector<bool> set(size);

    vector<int> indices(size);
    iota(indices.begin(), indices.end(), 0);
    default_random_engine rng;
    shuffle(indices.begin(), indices.end(), rng);

    vector<int> insert_indices(indices.begin(), indices.begin() + ratio*size);

    shuffle(indices.begin(), indices.end(), rng);

    for (auto _ : state) {
        for (auto i : indices) {
            benchmark::DoNotOptimize(set[i]);
        }
    }
}

void UnorderedMapClear_1(benchmark::State& state) {
    const int size = state.range(0);
    const double ratio = 0.1 * state.range(1);
    vector<bool> set(size);

    vector<int> indices(size);
    iota(indices.begin(), indices.end(), 0);
    default_random_engine rng;
    shuffle(indices.begin(), indices.end(), rng);

    vector<int> insert_indices(indices.begin(), indices.begin() + ratio*size);

    shuffle(indices.begin(), indices.end(), rng);

    for (auto _ : state) {
        fill(set.begin(), set.end(), false);
        for (auto i : insert_indices) set[i] = true;

        for (auto i : indices) {
            benchmark::DoNotOptimize(set[i]);
        }
    }
}

void UnorderedMap_2(benchmark::State& state) {
    const int size = state.range(0);
    const double ratio = 0.1 * state.range(1);
    unordered_set<int> set;

    vector<int> indices(size);
    iota(indices.begin(), indices.end(), 0);
    default_random_engine rng;
    shuffle(indices.begin(), indices.end(), rng);

    vector<int> insert_indices(indices.begin(), indices.begin() + ratio*size);

    shuffle(indices.begin(), indices.end(), rng);

    for (auto i : insert_indices) {
        set.insert(i);
    }

    for (auto _ : state) {
        for (auto i : indices) {
            benchmark::DoNotOptimize(set.find(i));
        }
    }
}

void UnorderedMapClear_2(benchmark::State& state) {
    const int size = state.range(0);
    const double ratio = 0.1 * state.range(1);
    unordered_set<int> set;

    vector<int> indices(size);
    iota(indices.begin(), indices.end(), 0);
    default_random_engine rng;
    shuffle(indices.begin(), indices.end(), rng);

    vector<int> insert_indices(indices.begin(), indices.begin() + ratio*size);

    shuffle(indices.begin(), indices.end(), rng);

    for (auto i : insert_indices) {
        set.insert(i);
    }

    for (auto _ : state) {
        set.clear();
        for (auto i : insert_indices) set.insert(i);

        for (auto i : indices) {
            benchmark::DoNotOptimize(set.find(i));
        }
    }
}


static void CustomArguments(benchmark::internal::Benchmark* b) {
    for (int i : {500, 1000, 2000, 5000})
        for (int j : {1, 3, 5, 7})
            b->Args({i, j});
}

BENCHMARK(UnorderedMap_1)->Apply(CustomArguments);
BENCHMARK(UnorderedMapClear_1)->Apply(CustomArguments);
BENCHMARK(UnorderedMap_2)->Apply(CustomArguments);
BENCHMARK(UnorderedMapClear_2)->Apply(CustomArguments);





