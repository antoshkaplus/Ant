
#include <urcu-qsbr.h>
#include <benchmark/benchmark.h>

static void RcuThreadOnline(benchmark::State& state) {

    rcu_register_thread();

    while (state.KeepRunning()) {

        state.PauseTiming();
        rcu_thread_offline();
        state.ResumeTiming();

        rcu_thread_online();
    }
    rcu_unregister_thread();
}

static void RcuThreadOffline(benchmark::State& state) {

    rcu_register_thread();

    while (state.KeepRunning()) {

        rcu_thread_offline();

        state.PauseTiming();
        rcu_thread_online();
        state.ResumeTiming();
    }
    rcu_unregister_thread();
}

BENCHMARK(RcuThreadOnline);
BENCHMARK(RcuThreadOffline);