
#include "gtest/gtest.h"

#include "ant/core/range/range.hpp"


using namespace ant;
using namespace std;


TEST(FilterIterator, _1) {
    std::vector<int> vs;
    auto range = ant::core::range::FilterRange(vs, TrueFunctor);
    ASSERT_EQ(ant::core::range::Count(range), 0);
}

TEST(FilterIterator, _2) {
    std::vector<int> vs = {0, 1, 2, 3, 4, 5};
    auto even = [](auto i) {return i%2==0;};
    {
        auto range = ant::core::range::FilterRange(vs, even);
        ASSERT_EQ(ant::core::range::Count(range), 3);
    }
    {
        for (auto _ : vs) if (std::not_fn(even)(_)) Println(cout, _);

        auto range = ant::core::range::FilterRange(vs, std::not_fn(even));
        ASSERT_EQ(ant::core::range::Count(range), 3);
    }

}