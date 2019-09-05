
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

struct TestParams_1 {
    uint32_t buffer_alloc_items;
    uint32_t slots;
    uint32_t items;
    uint32_t alloc_items;
    uint32_t iterations;
};

struct TestParams_2 {
    uint32_t buffer_alloc_items;
    uint32_t slots;
    uint32_t items;
    uint32_t alloc_items_min_count;
    uint32_t alloc_items_max_count;
    uint32_t iterations;
};


template <typename T>
class RingResourceTest : public testing::Test {

public:
    std::vector<alloc::vector<T>> test;
    std::vector<std::vector<T>> correct;

    std::default_random_engine rng;
    std::uniform_int_distribution<uint32_t> item_index_distr;
    std::uniform_int_distribution<uint32_t> alloc_items_count_distr;

    void SetUp(const TestParams_1& params) {
        test.assign(params.items, {});
        correct.assign(params.items, {});

        item_index_distr = std::uniform_int_distribution<uint32_t>{0, params.items - 1};
    }

    void SetUp(const TestParams_2& params) {
        test.assign(params.items, {});
        correct.assign(params.items, {});

        item_index_distr = std::uniform_int_distribution<uint32_t>{0, params.items - 1};

        alloc_items_count_distr = std::uniform_int_distribution{
                params.alloc_items_min_count, params.alloc_items_max_count};
    }

    void TearDown() {
        test = {};
    }

    void AssignAt(Index index, const std::vector<T>& new_item) {
        test[index] = alloc::vector<T>();
        test[index].assign(new_item.begin(), new_item.end());
        correct[index] = new_item;
    }

    void AssignAt(Index index, Count sz, T filling) {
        AssignAt(index, std::vector<T>(sz, filling));
    }

    void AssignAt(Index index, Count sz) {
        std::vector<T> vs(sz);
        std::for_each(vs.begin(), vs.end(), [this](auto& p) { p = GetRandom(); });
        AssignAt(index, vs);
    }

    bool IsEqual() {
        return std::equal(correct.begin(), correct.end(), test.begin());
    }

    T GetRandom() {
        if constexpr (std::is_same<T, int8_t>::value) {
            return  std::uniform_int_distribution<int8_t>()(rng);
        } else if constexpr (std::is_same<T, int32_t>::value) {
            return std::uniform_int_distribution<int32_t>()(rng);
        } else if constexpr (std::is_same<T, int64_t>::value) {
            return std::uniform_int_distribution<int64_t>()(rng);
        } else if constexpr (std::is_same<T, std::array<int64_t, 4>>::value) {
            return {
                std::uniform_int_distribution<int64_t>()(rng),
                std::uniform_int_distribution<int64_t>()(rng),
                std::uniform_int_distribution<int64_t>()(rng),
                std::uniform_int_distribution<int64_t>()(rng)
            };
        } else {
            static_assert("type not sypported");
        }
    }
};


using RingResourceTest_Types = ::testing::Types<int8_t, int32_t, int64_t, std::array<int64_t, 4>>;
TYPED_TEST_SUITE(RingResourceTest, RingResourceTest_Types);


void PrintAllocations(const RingResourceBase& resource) {
    auto str = ant::Format("local alloc: %d, dealloc: %d, slot releases: %d; standard alloc: %d, dealloc: %d",
                           resource.local_allocations(), resource.local_deallocations(),
                           resource.local_slot_releases(),
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
            RingResourceBase resource(p.buffer, p.slots);
            DefaultResourceGuard guard(resource);
            {
                alloc::vector<TypeParam> arr;
                arr.resize(10);
                FAIL();
            }
        } catch (...) {}
    }
}

TYPED_TEST(RingResourceTest, AllocationUsed) {
    RingResourceBase resource(sizeof(TypeParam) * 10, 1);
    DefaultResourceGuard guard(resource);
    {
        alloc::vector<TypeParam> arr;
        arr.resize(1);
    }
    ASSERT_TRUE(resource.local_allocations() == 1 &&
                resource.local_deallocations() == 1 &&
                resource.standard_allocations() == 0 &&
                resource.standard_deallocations() == 0 &&
                resource.local_slot_releases() == 1);
}

TYPED_TEST(RingResourceTest, OrderedStream_OneSize) {
    auto& rng = this->rng;
    auto& test = this->test;
    auto& correct = this->correct;

    std::vector<TestParams_1> params = {
            TestParams_1 {.buffer_alloc_items=10, .slots=2, .items=5, .alloc_items=1, .iterations=1000},
            TestParams_1 {.buffer_alloc_items=1000, .slots=100, .items=10, .alloc_items=50, .iterations=10000},
            TestParams_1 {.buffer_alloc_items=100, .slots=100, .items=1000, .alloc_items=20, .iterations=10000 }};

    for (auto& p : params) {
        RingResourceBase resource(sizeof(TypeParam) * p.buffer_alloc_items, p.slots);
        DefaultResourceGuard guard(resource);
        {
            this->SetUp(p);
            for (auto i = 0; i < p.iterations; ++i) {
                this->AssignAt(i % p.items, p.alloc_items);
                ASSERT_TRUE(this->IsEqual());
                ASSERT_EQ(resource.local_slot_releases(), resource.local_deallocations());
            }
            this->TearDown();
        }
        PrintAllocations(resource);
    }
}

TYPED_TEST(RingResourceTest, OrderedStream_RandomSize) {
    auto& rng = this->rng;
    auto& test = this->test;
    auto& correct = this->correct;

    std::vector<TestParams_2> params = {
            TestParams_2 {.buffer_alloc_items=10, .slots=2, .items=5,
                          .alloc_items_min_count=0, .alloc_items_max_count=5, .iterations=1000},
            TestParams_2 {.buffer_alloc_items=1000, .slots=100, .items=10,
                          .alloc_items_min_count=0, .alloc_items_max_count=50, .iterations=10000},
            TestParams_2 {.buffer_alloc_items=100, .slots=100, .items=1000,
                          .alloc_items_min_count=0, .alloc_items_max_count=20, .iterations=10000 }};

    for (auto& p : params) {
        RingResourceBase resource(sizeof(TypeParam) * p.buffer_alloc_items, p.slots);
        DefaultResourceGuard guard(resource);
        {
            this->SetUp(p);
            for (auto i = 0; i < p.iterations; ++i) {
                this->AssignAt(i % p.items, this->alloc_items_count_distr(rng));
                ASSERT_TRUE(this->IsEqual());
                ASSERT_EQ(resource.local_slot_releases(), resource.local_deallocations());
            }
            this->TearDown();
        }
        PrintAllocations(resource);
    }
}

TYPED_TEST(RingResourceTest, UnorderedStream_OneSize) {
    auto& rng = this->rng;
    auto& test = this->test;
    auto& correct = this->correct;

    std::vector<TestParams_1> params = {
            TestParams_1 {.buffer_alloc_items=10, .slots=2, .items=5, .alloc_items=1, .iterations=1000},
            TestParams_1 {.buffer_alloc_items=1000, .slots=100, .items=10, .alloc_items=50, .iterations=10000},
            TestParams_1 {.buffer_alloc_items=100, .slots=100, .items=1000, .alloc_items=20, .iterations=10000 }};

    for (auto& p : params) {
        RingResourceBase resource(2000 * 90, 10);
        DefaultResourceGuard guard(resource);
        {
            this->SetUp(p);
            for (auto i = 0; i < p.iterations; ++i) {
                this->AssignAt(this->item_index_distr(rng), p.alloc_items);
                ASSERT_TRUE(this->IsEqual());
            }
            this->TearDown();
        }
        PrintAllocations(resource);
    }
}


TYPED_TEST(RingResourceTest, UnorderedStream_RandomSize) {
    auto& rng = this->rng;
    auto& test = this->test;
    auto& correct = this->correct;

    std::vector<TestParams_2> params = {
            TestParams_2 {.buffer_alloc_items=10, .slots=2, .items=5,
                    .alloc_items_min_count=0, .alloc_items_max_count=5, .iterations=1000},
            TestParams_2 {.buffer_alloc_items=1000, .slots=100, .items=10,
                    .alloc_items_min_count=0, .alloc_items_max_count=50, .iterations=10000},
            TestParams_2 {.buffer_alloc_items=100, .slots=100, .items=1000,
                    .alloc_items_min_count=0, .alloc_items_max_count=20, .iterations=10000},
            TestParams_2 {.buffer_alloc_items=10000, .slots=1000, .items=1000,
                    .alloc_items_min_count=1, .alloc_items_max_count=10, .iterations=100000},
            TestParams_2 {.buffer_alloc_items=10000, .slots=1000, .items=100,
                    .alloc_items_min_count=1, .alloc_items_max_count=10, .iterations=100000}};

    for (auto& p : params) {
        RingResourceBase resource(sizeof(TypeParam) * p.buffer_alloc_items, p.slots);
        DefaultResourceGuard guard(resource);
        {
            this->SetUp(p);
            for (auto i = 0; i < p.iterations; ++i) {
                this->AssignAt(this->item_index_distr(rng), this->alloc_items_count_distr(rng));
                ASSERT_TRUE(this->IsEqual());
            }
            this->TearDown();
        }
        PrintAllocations(resource);
    }
}

}