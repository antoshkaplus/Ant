
#include <fstream>
#include <array>
#include <functional>
#include <set>
#include <thread>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/event_throttle.hpp"

namespace {

using namespace ant;
using namespace std;

constexpr int kIterations = 1000;

auto now() {
    return std::chrono::steady_clock::now();
}

TEST(EventThrottle, LimitZero) {
    EventThrottle throttle {0, std::chrono::milliseconds{1}};
    ASSERT_FALSE(throttle.can_authorize(now(), 1));
}

TEST(EventThrottle, LimitOne) {
    EventThrottle throttle {1, std::chrono::milliseconds{1}};
    for (auto i = 0; i < kIterations; ++i) {
        ASSERT_TRUE(throttle.can_authorize(now(), 1));
        throttle.Authorize(now());
        ASSERT_FALSE(throttle.can_authorize(now(), 1));
        std::this_thread::sleep_for(throttle.interval());
    }
}

TEST(EventThrottle, Burst) {
    EventThrottle throttle {10, std::chrono::milliseconds{1}};
    for (auto i = 0; i < kIterations; ++i) {
        for (auto k = 0; k < 10; ++k) {
            ASSERT_TRUE(throttle.can_authorize(now(), 10-k));
            throttle.Authorize(now());
        }
        ASSERT_FALSE(throttle.can_authorize(now(), 1));
        std::this_thread::sleep_for(throttle.interval());
    }
}

TEST(EventThrottle, TwoStepBurst) {
    std::chrono::milliseconds interval {2};
    std::chrono::milliseconds half_interval {1};

    EventThrottle throttle {10, interval};
    for (auto i = 0; i < kIterations; ++i) {
        for (auto k = 0; k < 5; ++k) {
            ASSERT_TRUE(throttle.can_authorize(now(), 10-k));
            throttle.Authorize(now());
        }
        std::this_thread::sleep_for(half_interval);
        for (auto k = 5; k < 10; ++k) {
            ASSERT_TRUE(throttle.can_authorize(now(), 10-k));
            throttle.Authorize(now());
        }
        ASSERT_FALSE(throttle.can_authorize(now(), 1));
        std::this_thread::sleep_for(interval);
    }
}

}