

#include "gtest/gtest.h"

#include "ant/core/skip_list_counter.hpp"
#include "ant/core/skip_list_set.hpp"
#include "ant/core/skip_list_op.hpp"

using namespace ant;
using namespace std;


TEST(SkipListSet, )


TEST(SkipListSet, allin) {
    int opCount = 10000;

    // we do a bunch of operations on set, vector and skip_list
    vector<int> vs;
    set<int> ss;
    SkipListSet<int> sls(opCount);

    int min_value = 1;
    int max_value = 10000;

    uniform_int_distribution<> value_distr(min_value, max_value);

    default_random_engine rng;
    discrete_distribution<int> op_distr({0.5, 0.2});

    int op;
    for (auto i = 0; i < opCount; ++i) {
        // 0 - push_back 0.5
        // 1 - get|remove at position and swap 0.2
        // at the end get|remove everything try to access after removal
        op = op_distr(rng);
        if (vs.empty()) {
            op = 0;
        }

        switch (op) {
            case 0: {
                auto val = value_distr(rng);
                if (ss.count(val) != 1) {
                    vs.push_back(val);
                    ss.insert(val);
                    sls.insert(val);
                }
                break;
            }
            case 1: {
                uniform_int_distribution<> pos_distr(0, vs.size()-1);
                auto pos = pos_distr(rng);
                auto val = vs[pos];
                SwapBackPop(vs, pos);
                ss.erase(val);
                ASSERT_EQ(sls[val], val);
                sls.remove(val);
                break;
            }
            default:
                throw runtime_error("unknown operation");
        }
    }
    for (auto v : vs) {
        sls.remove(v);
        ASSERT_EQ(sls.at(v), nullptr);
    }
}

