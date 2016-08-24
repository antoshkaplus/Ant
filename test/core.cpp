

#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"
#include "ant/core/avl_tree.hpp"
#include "ant/core/bst.hpp"
#include "ant/core/range_minmax.hpp"

namespace {


using namespace ant;
using namespace std;

TEST(AvlSet, allin) {

}


TEST(HeapFind, allin) {
    
    set<int> s;
    HeapFind<int> h;
    
    Count N = 100000;
    vector<int> gr(N);
    uniform_int_distribution<> distr;

    default_random_engine rng;
    // put some data inside
    for (auto i = 0; i < N; ++i) {
        while (true) {
            int g = distr(rng);
            if (!h.Exists(g)) {
                s.insert(g);
                h.Push(g);
                break;
            }
        }
        while (true) {
            int g = gr[i] = distr(rng);
            if (!h.Exists(g)) {
                s.insert(g);
                break;
            }
        }
    }
    for (auto g : gr) {
        s.erase(g);
    }
    
    shuffle(gr.begin(), gr.end(), rng);
    
    auto start_push = GetMillisCount();
    for (int g : gr) {
        h.Push(g);
    }
    cout << "PUSH HeapFind: " << GetMillisCount() - start_push << endl;
    start_push = GetMillisCount();
    for (int g : gr) {
        s.insert(g);
    }
    cout << "PUSH RBT: " << GetMillisCount() - start_push << endl;
    
    auto start_look_up = GetMillisCount();
    for (int g : gr) {
        h.Exists(g);
    }
    cout << "LOOK UP HeapFind: " << GetMillisCount() - start_look_up << endl;
    start_look_up = GetMillisCount();
    for (int g : gr) {
        s.count(g);
    }
    cout << "LOOK UP RBT: " << GetMillisCount() - start_look_up << endl; 
    
    auto start_pop = GetMillisCount(); 
    while (!h.Empty()) {
        h.Pop();
    }
    cout << "POP HeapFind: " << GetMillisCount() - start_pop << endl;
    start_pop = GetMillisCount();
    while (!s.empty()) {
        s.erase(s.begin());
    }
    cout << "POP RBT: " << GetMillisCount() - start_pop << endl;
}

TEST(CircularList, iteration) {
    CircularList<Count> list;
    CircularList<Count>::Iterator it = list.focus();
    list.InsertAfter(it, 89);
}

TEST(RangeMinimum, small) {
    vector<int> vs = {9, 5, 1, 0, 3, 4, 6, 20, 11};
    RangeMinimum<int> r(vs);
    ASSERT_EQ(0, r.Minimum(0, 4));
    ASSERT_EQ(1, r.Minimum(1, 2));
    ASSERT_EQ(r.Minimum(0, 1), 9);
    ASSERT_EQ(r.Minimum(1, 8), 0);
}

TEST(CountMap, allin) {
    {
        CountMap<int> counts;
        for (int i = 0; i < 10000; ++i) {
            counts.increase(i);
        }
    }
    {
        CountMap<int, UnorderedMap> counts;
        for (int i = 0; i < 10000; ++i) {
            counts.increase(i);
        }
    }
}

TEST(core, split_fixed) {
    char delim = ',';
    string str = "";
    auto res = Split(str, delim);
    ASSERT_TRUE(res.size() == 1 && res[0] == "");
    str = ",";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == "" && res[1] == "");
    str = " ,";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == " " && res[1] == "");
    str = ", ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == "" && res[1] == " ");
    str = " , ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == " " && res[1] == " ");
    str = " ,, ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 3 && res[0] == " " && res[1] == "" && res[2] == " ");
    str = " , , ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 3 && res[0] == " " && res[1] == " " && res[2] == " ");
}

TEST(core, split_variable) {
    char delim = ',';
    string str = "";
    auto res = Split(str, delim);
    ASSERT_TRUE(res.size() == 1 && res[0] == "");
    str = ",";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == "" && res[1] == "");
    str = " ,";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == " " && res[1] == "");
    str = ", ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == "" && res[1] == " ");
    str = " , ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 2 && res[0] == " " && res[1] == " ");
    str = " ,, ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 3 && res[0] == " " && res[1] == "" && res[2] == " ");
    str = " , , ";
    res = Split(str, delim);
    ASSERT_TRUE(res.size() == 3 && res[0] == " " && res[1] == " " && res[2] == " ");
}

TEST(CountDigits, all_in) {
    int64_t t = 1;
    for (auto i = 1; i <= 18; ++i) {
        Count count = CountDigits(t);
        ASSERT_EQ(count, i);
        t *= -10;
    }
    ASSERT_EQ(CountDigits(0), 0);
}

    
}



