
#include <thread>
#include <atomic>

#include <gtest/gtest.h>
#include <urcu-qsbr.h>

#include "ant/core/core.hpp"
#include "ant/parallel/rcu/unordered_map.hpp"


TEST(RcuUnorderedMap, _1) {
    rcu_register_thread();

    ant::parallel::rcu::unordered_map<int, std::string> map;

    auto k_1 = 1;
    auto k_2 = 2;

    auto v_1 = "cat";
    auto v_2 = "dog";

    map[k_1] = v_1;
    map[k_2] = v_2;

    rcu_barrier();
    rcu_unregister_thread();
}