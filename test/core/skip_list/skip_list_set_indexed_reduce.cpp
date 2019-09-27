//
//
//#include "gtest/gtest.h"
//
//#include "test/core/set.hpp"
//#include "ant/core/skip_list/skip_list_set_indexed_reduce.hpp"
//
//using namespace ant;
//using namespace std;
//
//int sum(int i_1, int i_2) { return i_1 + i_2; }
//
//SkipListSetIndexedReduce<int, decltype(&sum)> MakeReduce(int max_elems) {
//    return SkipListSetIndexedReduce<int, decltype(&sum)>{max_elems, sum};
//}
//
////
////template <template <typename> typename Set_, ant::Count kSize_>
////struct SetTestCase {
////    template <typename Value> using Set = Set_<Value>;
////    constexpr static Count kSize = kSize_;
////};
////
////template <typename TestCase, typename Value>
////typename TestCase::template Set<Value> MakeSet(size_t capacity);
//
//
//TEST(SkipListSetIndexedReduce, constructor) {
//
//    auto s_0 = MakeReduce(0);
//    auto s_1 = MakeReduce(1);
//    auto s_2 = MakeReduce(2);
//}
//
//TEST(SkipListSetIndexedReduce, empty) {
//    auto s = MakeReduce(1);
//    ASSERT_TRUE(s.empty());
//
//    s.Insert(1);
//    s.Remove(2);
//    ASSERT_FALSE(s.empty());
//
//    s.Remove(1);
//    ASSERT_TRUE(s.empty());
//}
//
//TEST(SkipListSetIndexedReduce, ConstIterator) {
//    const auto s = MakeReduce(80);
//    for (auto& p : s) p;
//}
//
//class SkipListSetIndexedReduceTest : public testing::TestWithParam<ant::Count> {
//protected:
//    std::set<int> ss;
//    SkipListSetIndexedReduce<int, decltype(&sum)> skip_list = MakeReduce(1);
//
//    std::default_random_engine rng;
//
//    uniform_int_distribution<> index_distr;
//    std::uniform_int_distribution<> op_distr {0, 1};
//
//    void SetUp() override {
//        auto size = GetParam();
//
//        skip_list = MakeReduce(size);
//
//        index_distr = uniform_int_distribution<>{0, size-1};
//    }
//
//    void CheckEqual() {
//        ASSERT_EQ(ss.size(), skip_list.size());
//        ASSERT_TRUE(std::equal(ss.begin(), ss.end(), skip_list.begin()));
//    }
//};
////
////INSTANTIATE_TEST_SUITE_P(InstantiationName,
////                         SkipListSetIndexedReduceTest,
////                         testing::Values(1, 10, 100));
//
//TEST_P(SkipListSetIndexedReduceTest, Count) {
//    constexpr bool kMicroCheck = false;
//
//    const auto iterations = GetParam() * GetParam();
//
//    const auto INSERT = 0;
//    const auto REMOVE = 1;
//
//    for (auto i = 0; i < iterations; ++i) {
//        auto op = op_distr(rng);
//        switch (op) {
//            case INSERT: {
//                for (auto n = 0, N = index_distr(rng); n < N; ++n) {
//                    auto k = index_distr(rng);
//
//                    ss.insert(k);
//                    skip_list.Insert(k);
//
//                    ASSERT_TRUE(skip_list.Count(k) == 1);
//
//                    if constexpr (kMicroCheck) CheckEqual();
//                }
//                break;
//            }
//            case REMOVE: {
//                for (auto n = 0, N = index_distr(rng); n < N; ++n) {
//                    auto k = index_distr(rng);
//
//                    ss.erase(k);
//                    skip_list.Remove(k);
//
//                    ASSERT_TRUE(skip_list.Count(k) == 0);
//
//                    if constexpr (kMicroCheck) CheckEqual();
//                }
//                break;
//            }
//            default:
//                throw runtime_error("unknown operation");
//        }
//
//        if constexpr (!kMicroCheck) CheckEqual();
//    }
//}
//
