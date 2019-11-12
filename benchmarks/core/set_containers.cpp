
#include <ant/core/skip_list/skip_list_set.hpp>
#include "benchmark/benchmark.h"

#include "ant/core/core.hpp"
#include "ant/core/flat_set.hpp"

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
void Insert(ant::SkipListSet<uint64_t>& s, int64_t n) {
    s.Insert(n);
}

void Delete(std::set<uint64_t>& s, uint64_t n) {
    s.erase(n);
}
void Delete(ant::FlatSet<uint64_t>& s, uint64_t n) {
    s.Erase(n);
}
void Delete(ant::SkipListSet<uint64_t>& s, uint64_t n) {
    s.Remove(n);
}
size_t Count(std::set<uint64_t>& s, uint64_t n) {
    return s.count(n);
}
size_t Count(ant::SkipListSet<uint64_t>& s, uint64_t n) {
    return s.Count(n);
}

template<class Set, class GenFunc>
void SetInsert(benchmark::State& state, Set init_set, GenFunc genFunc) {
    auto numbers = genFunc(state);
    for (auto _ : state) {
        Set set = init_set;
        for (auto n : numbers) {
            Insert(set, n);
        }
    }
}


typedef std::vector<uint64_t>(*Func)(benchmark::State&);

BENCHMARK_CAPTURE(SetInsert, SetSorted, std::set<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, FlatSetSorted, ant::FlatSet<uint64_t>(), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, SkipListSetSorted, ant::SkipListSet<uint64_t>(3000), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetInsert, SetReversed, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, FlatSetReversed, ant::FlatSet<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetInsert, SkipListSetReversed, ant::SkipListSet<uint64_t>(3000), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetInsert, SetPermutated, std::set<uint64_t>(), Func(PermutatedNumbers))
->Args({500, 500/4})->Args({1000, 1000/4})->Args({2000, 2000/4})->Args({3000, 3000/4});
BENCHMARK_CAPTURE(SetInsert, FlatSetPermutated, ant::FlatSet<uint64_t>(), Func(PermutatedNumbers))
->Args({500, 500/4})->Args({1000, 1000/4})->Args({2000, 2000/4})->Args({3000, 3000/4});
BENCHMARK_CAPTURE(SetInsert, SkipListSetPermutated, ant::SkipListSet<uint64_t>(3000), Func(PermutatedNumbers))
->Args({500, 500/4})->Args({1000, 1000/4})->Args({2000, 2000/4})->Args({3000, 3000/4});


template<class Set, class GenFunc>
void SetDelete(benchmark::State& state, Set init_set, GenFunc genFunc) {
    auto numbers = genFunc(state);
    for (auto _ : state) {
        state.PauseTiming();
        Set set = init_set;
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
BENCHMARK_CAPTURE(SetDelete, SkipListSetSorted, ant::SkipListSet<uint64_t>(3000), Func(SortedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetDelete, SetReversed, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetDelete, FlatSetReversed, ant::FlatSet<uint64_t>(), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);
BENCHMARK_CAPTURE(SetDelete, SkipListSetReversed, ant::SkipListSet<uint64_t>(3000), Func(ReversedNumbers))->Arg(500)->Arg(1000)->Arg(2000)->Arg(3000);

BENCHMARK_CAPTURE(SetDelete, SetPermutated, std::set<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500})->Args({1000, 1000})->Args({2000, 2000})->Args({3000, 3000});
BENCHMARK_CAPTURE(SetDelete, FlatSetPermutated, ant::FlatSet<uint64_t>(), Func(PermutatedNumbers))->Args({500, 500})->Args({1000, 1000})->Args({2000, 2000})->Args({3000, 3000});
BENCHMARK_CAPTURE(SetDelete, SkipListSetPermutated, ant::SkipListSet<uint64_t>(3000), Func(PermutatedNumbers))->Args({500, 500})->Args({1000, 1000})->Args({2000, 2000})->Args({3000, 3000});



BENCHMARK_CAPTURE(SetInsert, SetSorted_Big, std::set<uint64_t>(), Func(SortedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetInsert, SkipListSetSorted_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(SortedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);

BENCHMARK_CAPTURE(SetInsert, SetReversed_Big, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetInsert, SkipListSetReversed_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(ReversedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);

BENCHMARK_CAPTURE(SetInsert, SetPermutated_Big, std::set<uint64_t>(), Func(PermutatedNumbers))
->Args({100'000, 100'000/4})->Args({1000'000, 1000'000/4})->Args({10'000'000, 10'000'000/4});
BENCHMARK_CAPTURE(SetInsert, SkipListSetPermutated_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(PermutatedNumbers))
->Args({100'000, 100'000/4})->Args({1000'000, 1000'000/4})->Args({10'000'000, 10'000'000/4});


BENCHMARK_CAPTURE(SetDelete, SetSorted_Big, std::set<uint64_t>(), Func(SortedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetDelete, SkipListSetSorted_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(SortedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);

BENCHMARK_CAPTURE(SetDelete, SetReversed_Big, std::set<uint64_t>(), Func(ReversedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetDelete, SkipListSetReversed_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(ReversedNumbers))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);

BENCHMARK_CAPTURE(SetDelete, SetPermutated_Big, std::set<uint64_t>(), Func(PermutatedNumbers))
->Args({100'000, 100'000})->Args({1000'000, 1000'000})->Args({10'000'000, 10'000'000});
BENCHMARK_CAPTURE(SetDelete, SkipListSetPermutated_Big, ant::SkipListSet<uint64_t>(10'000'000), Func(PermutatedNumbers))
->Args({100'000, 100'000})->Args({1000'000, 1000'000})->Args({10'000'000, 10'000'000});



template<class Set>
void SetTraversal(benchmark::State& state, Set init_set) {
    std::vector<uint64_t> numbers(state.range(0));
    std::iota(numbers.begin(), numbers.end(), 0);
    std::default_random_engine rng;
    for (auto _ : state) {
        state.PauseTiming();
        Set set = init_set;
        std::shuffle(numbers.begin(), numbers.end(), rng);
        for (auto n : numbers) {
            Insert(set, n);
        }
        state.ResumeTiming();

        for (auto& p : set) {
            benchmark::DoNotOptimize(p);
        }
        state.PauseTiming();
    }
}

BENCHMARK_CAPTURE(SetTraversal, Set_Big, std::set<uint64_t>())->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetTraversal, SkipListSet_Big, ant::SkipListSet<uint64_t>(10'000'000))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);


template<class Set>
void SetLookup(benchmark::State& state, Set init_set) {
    std::vector<uint64_t> numbers(state.range(0));
    std::iota(numbers.begin(), numbers.end(), 0);
    std::default_random_engine rng;
    for (auto _ : state) {
        state.PauseTiming();
        Set set = init_set;
        std::shuffle(numbers.begin(), numbers.end(), rng);
        for (auto n : numbers) {
            Insert(set, n);
        }

        std::shuffle(numbers.begin(), numbers.end(), rng);
        state.ResumeTiming();

        for (auto n : numbers) {
            benchmark::DoNotOptimize(Count(set, n));
        }
        state.PauseTiming();
    }
}

BENCHMARK_CAPTURE(SetLookup, Set_Big, std::set<uint64_t>())->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);
BENCHMARK_CAPTURE(SetLookup, SkipListSet_Big, ant::SkipListSet<uint64_t>(10'000'000))->Arg(100'000)->Arg(1000'000)->Arg(10'000'000);