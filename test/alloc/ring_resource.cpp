
#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/alloc/alloc.hpp"
#include "ant/alloc/ring_resource.hpp"

namespace {

using namespace ant;
using namespace alloc;
using namespace std;


TEST(RingResource, allin) {
    RingResource<uint16_t> resource(2000 * 90, 1000);

    std::vector<alloc::vector<int>> vs(10, alloc::vector<int>(&resource));
    for (auto i = 0; i < vs.size(); ++i) {
        vs[i].resize(10, i);
    }

    for (auto i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(vs[i].size(), 10);
        for (auto k : vs[i]) ASSERT_EQ(k, i);
    }
}

TEST(RingResource, _1) {
    RingResource<uint16_t> resource(2000 * 90, 1000);

    std::vector<alloc::vector<int>> vs(10, alloc::vector<int>(&resource));
    for (auto i = 0; i < vs.size(); ++i) {
        vs[i].resize(10, i);
    }

    for (auto i = 0; i < vs.size(); ++i) {
        ASSERT_EQ(vs[i].size(), 10);
        for (auto k : vs[i]) ASSERT_EQ(k, i);
    }
}


}