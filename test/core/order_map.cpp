
#include "gtest/gtest.h"

#include "ant/core/order_map.hpp"


using namespace ant;
using namespace std;


TEST(OrderMap, allin) {

    const int N = 10;
    vector<int> arr(N);
    default_random_engine rng;
    iota(arr.begin(), arr.end(), 0);

    OrderMap<int> map;
    for (auto i = 0; i < 100; ++i) {
        vector<int> arr_1 = arr;
        vector<int> arr_2 = arr;
        MakeOrderMap(arr_1);
        map.Make(arr_2);

        if (arr_1 != arr_2) {
            Println(cout, "main: ", arr);
            Println(cout, "arr_1: ", arr_1);
            Println(cout, "arr_2: ", arr_2);
        }
        ASSERT_EQ(arr_1, arr_2);

        shuffle(arr.begin(), arr.end(), rng);
    }
}