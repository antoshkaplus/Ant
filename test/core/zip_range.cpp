#include <random>

#include "gtest/gtest.h"

#include "ant/core/core.hpp"


namespace {

using namespace ant;
using namespace std;

TEST(ZipRange, _1) {

    std::vector<int> zeros(10, 0);
    int i = 0;
    for (auto [z_i, z_1, z_2] : iZipRange(zeros, zeros)) {
        ASSERT_TRUE(z_i == i++ && z_1 == 0 && z_2 == 0) << i << " " << z_i << " " << z_1 << " " << z_2;
    }
}

TEST(ZipRange, _2) {

    std::vector<int> zeros(10, 0);
    std::vector<int> ones (10, 1);
    int i = 0;
    for (auto [z_i, z_1, z_2] : iZipRange(zeros, ones)) {
        ASSERT_TRUE(z_i == i++ && z_1 == 0 && z_2 == 1) << i << " " << z_i << " " << z_1 << " " << z_2;
    }
}

TEST(ZipRange, _3) {

    std::vector<int> index(10, 0);
    std::iota(index.begin(), index.end(), 0);
    int i = 0;
    for (auto [z_i, z_1, z_2] : iZipRange(index, index)) {
        ASSERT_TRUE(z_i == i && z_1 == i && z_2 == i++) << i << " " << z_i << " " << z_1 << " " << z_2;
    }
}

}