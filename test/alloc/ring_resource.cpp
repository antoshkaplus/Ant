
#include <fstream>
#include <array>
#include <functional>
#include <set>
#include <random>
#include <vector>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/alloc/alloc.hpp"
#include "ant/alloc/ring_resource.hpp"

namespace {

using namespace ant;
using namespace alloc;
using namespace std;


struct TestParams {
    uint32_t slots;
    uint32_t alloc_items;
    uint32_t items;
    uint32_t alloc_item_min;
    uint32_t alloc_item_max;
    uint32_t alloc_items_min_count;
    uint32_t alloc_items_max_count;
};


template <typename T>
class RingResourceTest : public testing::TestWithParam<TestParams> {

    std::vector<alloc::vector<T>> test;
    std::vector<std::vector<T>> correct;

    std::default_random_engine rng;
    std::uniform_int_distribution<uint32_t> item_index_distr;
    std::uniform_int_distribution<uint32_t> alloc_items_count_distr;
    std::uniform_int_distribution<uint32_t> alloc_item_distr;

    virtual void SetUp() {
        auto param = GetParam();
        test.assign(param.items, {});
        correct.assign(param.items, {});

        item_index_distr = std::uniform_int_distribution<uint32_t>{0, param.items - 1};
        alloc_items_count_distr = std::uniform_int_distribution{
            param.alloc_items_min_count, param.alloc_items_max_count};
        alloc_item_distr = std::uniform_int_distribution{
            param.alloc_item_min, param.alloc_item_max};
    }


};



using RingResourceTest_Types = ::testing::Types<int8_t, int32_t, int64_t, std::array<int64_t, 4>>;
TYPED_TEST_SUITE(RingResourceTest, RingResourceTest_Types);


void PrintAllocations(const RingResource& resource) {
    auto str = ant::Format("local alloc: %d, dealloc: %d; standard alloc: %d, dealloc: %d",
                           resource.local_allocations(), resource.local_deallocations(),
                           resource.standard_allocations(), resource.standard_deallocations());
    Println(std::cout, str);
}

struct RingResourceSize {
    uint32_t buffer;
    uint32_t slots;
};


TYPED_TEST(RingResourceTest, ZeroBufferSlots) {
    std::vector<RingResourceSize> params = {{0, 0}, {0, 10}, {10, 0}, {1, 10}};
    for (auto& p : params) {
        try {
            RingResource resource(p.buffer, p.slots);
            DefaultResourceGuard guard(resource);
            {
                alloc::vector<TypeParam> arr;
                arr.resize(10);
            }
        } catch (...) {}
    }
}

TYPED_TEST(RingResourceTest, ContiniousAllocation) {
    RingResource resource(2000 * 90, 1000);
    DefaultResourceGuard guard(resource);
    {
        const auto kItemCount = 1000;
    }
}


//
//TEST(RingResourceTest, RandomVectorAssignment_Small) {
//    RingResource resource(2000 * 90, 10);
//    DefaultResourceGuard guard(resource);
//    {
//        const auto kItemCount = 1000;
//        std::vector<alloc::vector<int>> test(kItemCount, alloc::vector<int>(&resource));
//        std::vector<std::vector<int>> correct(kItemCount);
//
//        const auto kItem_2_CountMin = 0;
//        const auto kItem_2_CountMax = 10;
//
//        const auto kItem_2_Min = 0;
//        const auto kItem_2_Max = (int) 10e+6;
//
//        const auto iterations = (int) 10e+3;
//    }
//}



TEST(RingResourceTest, RandomVectorAssignment_Big) {
    RingResource resource(2000 * 90, 1000);
    DefaultResourceGuard guard(resource);
    {
        const auto kItemCount = 1000;
        std::vector<alloc::vector<int>> test(kItemCount, alloc::vector<int>(&resource));
        std::vector<std::vector<int>> correct(kItemCount);

        const auto kItem_2_CountMin = 0;
        const auto kItem_2_CountMax = 10;

        const auto kItem_2_Min = 0;
        const auto kItem_2_Max = (int) 10e+6;

        const auto iterations = (int) 10e+3;

        for (auto i = 0; i < iterations; ++i) {
            std::vector<int> new_item(count_distr(rng));
            for (auto& elem : new_item) elem = item_distr(rng);

            auto index = item_index_distr(rng);
            test[index] = alloc::vector<int>();
            test[index].assign(new_item.begin(), new_item.end());
            correct[index] = new_item;

            if (!std::equal(correct.begin(), correct.end(), test.begin())) {
                PrintAllocations(resource);

                ASSERT_TRUE(false);
            }
        }
    }
    PrintAllocations(resource);
}

TEST(RingResourceTest, _1) {
    RingResource resource(2000 * 90, 1000);

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