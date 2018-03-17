
#include "benchmark/benchmark.h"

#include "ant/core/core.hpp"

std::vector<uint64_t> SortedNumbers(ant::Count count) {
    std::vector<uint64_t> numbers = std::vector<uint64_t>(count);
    std::iota(numbers.begin(), numbers.end(), 0);
    return numbers;
}
std::vector<uint64_t> SortedNumbers(benchmark::State& state) {
    return SortedNumbers(state.range(0));
}

std::vector<uint64_t> ReversedNumbers(ant::Count count) {
    auto numbers = SortedNumbers(count);
    std::reverse(numbers.begin(), numbers.end());
    return numbers;
}
std::vector<uint64_t> ReversedNumbers(benchmark::State& state) {
    return ReversedNumbers(state.range(0));
}

std::vector<uint64_t> PermutatedNumbers(ant::Count count, ant::Count perms) {
    auto numbers = SortedNumbers(count);

    std::default_random_engine rng;
    std::uniform_int_distribution<> distr(0, numbers.size()-1);
    for (auto i = 0; i < perms; ++i) {
        std::swap(numbers[distr(rng)], numbers[distr(rng)]);
    }
    return numbers;
}
std::vector<uint64_t> PermutatedNumbers(benchmark::State& state) {
    return PermutatedNumbers(state.range(0), state.range(1));
}

void Insert(std::set<uint64_t>& s, uint64_t n) {
    s.insert(n);
}
void Insert(ant::FlatSet<uint64_t>& s, uint64_t n) {
    s.Insert(n);
}
void Delete(std::set<uint64_t>& s, uint64_t n) {
    s.erase(n);
}
void Delete(ant::FlatSet<uint64_t>& s, uint64_t n) {
    s.Erase(n);
}


template<class Set, class GenFunc>
void SetInsert(benchmark::State& state, Set, GenFunc genFunc) {
    auto numbers = genFunc(state);
    for (auto _ : state) {
        Set set;
        for (auto n : numbers) {
            Insert(set, n);
        }
    }
}


typedef std::vector<uint64_t>(*Func)(benchmark::State&);

BENCHMARK_CAPTURE(SetInsert, SetSorted, std::set<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, FlatSetSorted, ant::FlatSet<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetInsert, SetReversed, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, FlatSetReversed, ant::FlatSet<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetInsert, SetPermutated, std::set<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500/4})->Args({1000, 1000/4})->Args({2000, 2000/4})->Args({3000, 3000/4});
BENCHMARK_CAPTURE(SetInsert, FlatSetPermutated, ant::FlatSet<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500/4})->Args({1000, 1000/4})->Args({2000, 2000/4})->Args({3000, 3000/4});

template<class Set, class GenFunc>
void SetDelete(benchmark::State& state, Set, GenFunc genFunc) {
    auto numbers = genFunc(state);
    for (auto _ : state) {
        state.PauseTiming();
        Set set;
        for (auto n : numbers) {
            Insert(set, n);
        }
        state.ResumeTiming();
        for (auto n : numbers) {
            Delete(set, n);
        }
    }
}

BENCHMARK_CAPTURE(SetDelete, SetSorted, std::set<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetDelete, FlatSetSorted, ant::FlatSet<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetDelete, SetReversed, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetDelete, FlatSetReversed, ant::FlatSet<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetDelete, SetPermutated, std::set<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500})->Args({1000, 1000})->Args({2000, 2000})->Args({3000, 3000});
BENCHMARK_CAPTURE(SetDelete, FlatSetPermutated, ant::FlatSet<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500})->Args({1000, 1000})->Args({2000, 2000})->Args({3000, 3000});
