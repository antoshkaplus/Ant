
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm>

#include "gtest/gtest.h"
#include "benchmark/benchmark.h"

#include "ant/core/core.hpp"

using namespace std;
using namespace ant;




TEST(life, map_vs_vector) {
    const static int kModifCount = 1000; 
    const static int kElemCount = 100;
    std::default_random_engine rng;
    
    
    std::vector<uint64_t> vv;
    auto start = chrono::system_clock::now();
    
    std::uniform_int_distribution<> elem_distr;
    for (int i = 0; i < kElemCount; ++i) {
        vv.push_back(elem_distr(rng));
    }
    
    
    for (int i = 0; i < kModifCount; ++i) {
        std::uniform_int_distribution<> distr(0, vv.size()-1);
        auto ind = distr(rng);
        auto v = vv[ind];
        remove(vv.begin(), vv.end(), v);
        vv.pop_back();
        
        for (auto v : vv) {
            // do something here
        }
        
        auto elem = elem_distr(rng);
        auto it = std::upper_bound(vv.begin(), vv.end(), elem);
        vv.insert(it, elem);
        
        for (auto v : vv) {
            // do something again
        }
    }
    
    //std::set<uint64_t> vv

}


default_random_engine RNG;

std::unordered_map<uint64_t, std::string> MakeProblem(int size) {
    std::uniform_int_distribution<uint64_t> distr;
    std::uniform_int_distribution<> alphabet('A', 'Z');

    std::unordered_map<uint64_t, std::string> items;
    for (auto i = 0; i < size; ++i) {
        string s(3, 'A');
        s[0] = alphabet(RNG);
        s[1] = alphabet(RNG);
        s[2] = alphabet(RNG);
        items[distr(RNG)] = s;
    }
    return items;
}

static void OuterDataSort(benchmark::State& state) {
    Count sz = state.range(0);

    auto pr = MakeProblem(sz);
    while (state.KeepRunning()) {
        std::vector<std::string> items;
        items.reserve(sz);
        for (auto& p : pr) {
            auto s = pr[p.first];
            items.emplace_back(std::move(s));
        }

        std::sort(items.begin(), items.end(), [](auto& p_1, auto& p_2)
        {
            return p_1 < p_2;
        });
    }
}

static void InnerDataSort(benchmark::State& state) {
    Count sz = state.range(0);

    auto pr = MakeProblem(sz);

    while (state.KeepRunning()) {
        std::vector<std::unordered_map<uint64_t, string>::const_iterator> items;
        items.reserve(sz);
        for(auto it = pr.begin(); it != pr.end(); ++it) {
            items.emplace_back(it);
        }

        std::sort(items.begin(), items.end(), [&](auto& it_1, auto& it_2) {
            auto& s_1 = pr[it_1->first];
            auto& s_2 = pr[it_2->first];
            return s_1 < s_2;
        });
    }
}

BENCHMARK(OuterDataSort)->Arg(25)->Arg(50)->Arg(100);
BENCHMARK(InnerDataSort)->Arg(25)->Arg(50)->Arg(100);


int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    cout << "working directory: " << argv[0] << endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
