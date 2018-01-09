
#include "gtest/gtest.h"

#include "ant/core/core.hpp"


namespace {

using namespace ant;
using namespace std;

TEST(VectorSet, all) {

    VectorSet<int> vs;

    vs.Insert(1);
    vs.Insert(2);
    vs.Insert(3);
    vs.Insert(4);
    vs.Insert(1);
    vs.Insert(4);
    ASSERT_EQ(vs.get().size(), 4);

    auto st = vs.Erase(1);
    ASSERT_TRUE(st.second);
    ASSERT_FALSE(vs.Erase(5).second);

    vs.Erase(st.first);
    ASSERT_EQ(vs.get().size(), 2);

    auto v = vs.Detach();
}


}