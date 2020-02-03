
#include "gtest/gtest.h"

#include "ant/core/flat_iterator.hpp"


using namespace ant;
using namespace std;


TEST(FlatIterator, _1) {
    std::vector<int> vs;
    FlatRange(vs.begin(), vs.end());
}

TEST(FlatIterator, _2) {
    std::unordered_map<int, int> vs;
    FlatRange(vs.begin(), vs.end());
}

TEST(FlatIterator, _3) {
    std::unordered_map<int, std::vector<int>> vs;
    FlatRange(vs.begin(), vs.end(), [](auto& p) { return p.second; });
}

TEST(FlatIterator, _4) {
    auto identity = [](auto& v) -> auto& { return v; };

    std::vector<int> vs_1{1, 2};
    {
        auto r = FlatRange(vs_1.begin(), vs_1.end());
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
    std::vector<std::vector<int>> vs_2{1, vs_1};
    {
        auto r = FlatRange(vs_2.begin(), vs_2.end(), identity);
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
    std::vector<std::vector<std::vector<int>>> vs_3{1, vs_2};
    {
        auto r = FlatRange(vs_3.begin(), vs_3.end(), identity, identity);
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
}

