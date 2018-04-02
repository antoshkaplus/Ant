
#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/alloc/local_resource.hpp"

namespace {

using namespace ant;
using namespace alloc;
using namespace std;


TEST(LocalResource, allin) {
    LocalResourceManager manager(10000);
    LocalResource resource(manager);

    std::vector<alloc::vector<int>> vs(10, alloc::vector<int>(&resource));
    for (auto i = 0; i < vs.size(); ++i) {
        vs[i].resize(10, i);
    }

    for (auto i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(vs[i].size(), 10);
        for (auto k : vs[i]) ASSERT_EQ(k, i);
    }
}

static void Recurse(int currentLevel, int stopLevel, LocalResourceManager& manager) {
    if (currentLevel == stopLevel) return;

    LocalResource resource(manager);

    Recurse(currentLevel+1, stopLevel, manager);

    alloc::unordered_map<int, int> map(&resource);
    for (auto i = 0; i < 1000; ++i) {
        map[i] = i;
    }

    for (auto i = 0; i < 1000; ++i) {
        ASSERT_EQ(map[i], i);
    }
}

TEST(LocalResource, Recursion) {

    LocalResourceManager manager(1000*64);
    Recurse(0, 10, manager);

}

TEST(LocalResource, AllocFailed) {

    LocalResourceManager manager(100*2);
    LocalResource resource(manager);

    alloc::vector<int>(10, 0, &resource);
    ASSERT_FALSE(manager.allocation_failed());
    alloc::vector<int>(100, 0, &resource);
    ASSERT_TRUE(manager.allocation_failed());
}

TEST(LocalResource, Release) {
    LocalResourceManager manager(100*2);
    LocalResource resource(manager);

    ASSERT_EQ(manager.used(), 0);

    alloc::vector<int>(10, 0, &resource);

    auto used = manager.used();
    {
        LocalResource resource_inner(manager);
        alloc::vector<int>(10, 0, &resource);
    }

    ASSERT_EQ(manager.used(), used);
}

}
