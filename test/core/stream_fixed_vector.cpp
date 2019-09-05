
#include <random>

#include "gtest/gtest.h"

#include "ant/core/stream_fixed_vector.hpp"


namespace {

using namespace ant;
using namespace std;

auto ch = []() { return 'a'; };

TEST(StreamFixedVector, Constructors) {
    StreamFixedVector<uint32_t, char> vector_1;
    StreamFixedVector<uint32_t, char> vector_2(10);
}

TEST(StreamFixedVector, size) {
    StreamFixedVector<uint32_t, char> vector(2);
    vector.Insert(0, ch());
    vector.Insert(1, ch());
    ASSERT_EQ(2, vector.size());

    vector.Insert(2, ch());
    ASSERT_EQ(1, vector.lagging_count());
    ASSERT_EQ(3, vector.size());
}

TEST(StreamFixedVector, InsertExtract) {
    StreamFixedVector<uint32_t, char> vector(3);
    for (auto i = 0; i < 10; ++i) vector.Insert(i, ch());

    ASSERT_EQ(7, vector.lagging_count());
    ASSERT_EQ(10, vector.size());

    for (auto i = 0; i < 10; ++i) vector.Extract(i);
    ASSERT_EQ(0, vector.size());
}

TEST(StreamFixedVector, RandomExtract) {
    StreamFixedVector<uint32_t, char> vector(100);
    constexpr int N = 1000000;
    constexpr int EMPTY = -1;
    std::vector<int> indices(N, EMPTY);
    std::default_random_engine rng;
    for (auto i = 0; i < N; ++i) {
        indices[i] = i;
        vector.Insert(i, ch());
        auto remove_index = std::uniform_int_distribution(0, i)(rng);
        if (indices[remove_index] != EMPTY) {
            vector.Extract(remove_index);
            indices[remove_index] = EMPTY;
        }
    }

    ASSERT_EQ(indices.size() - std::count(indices.begin(), indices.end(), EMPTY),
            vector.size());

    for(auto index : indices) {
        if (index != EMPTY) vector.Extract(index);
    }

    ASSERT_EQ(0, vector.size());
}

}
