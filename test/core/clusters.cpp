
#include "gtest/gtest.h"

#include "ant/core/core.hpp"


using namespace ant;
using namespace std;

struct TestCase {
    std::vector<Index> initial;
    std::vector<bool> used;
    std::vector<Index> expected;
};


TEST(DecreaseClustering, _1) {

    std::vector<TestCase> test_cases = {
        {
            {0, 1, 2},
            {true, true, true},
            {0, 1, 2}
        },
        {
            { 0, 1, 2 },
            { false, true, true },
            { -1, 0, 1 }
        },
        {
            { 0, 1, 2, 1, 0 },
            { false, true, true },
            { -1, 0, 1, 0, -1 }
        },
    };

    for (auto i = 0; i < test_cases.size(); ++i) {
        auto t = test_cases[i];
        auto res = t.initial;
        DecreaseClustering(res, t.used);
        ASSERT_EQ(res, t.expected) << "test index: " << i;
    }
}