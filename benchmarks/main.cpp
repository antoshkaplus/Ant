
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm>

#include "gtest/gtest.h"


using namespace std;





TEST(life, map_vs_vector) {
    const static int kModifCount = 1000; 
    const static int kElemCount = 100;
    std::default_random_engine rng;
    
    
    std::vector<uint64_t> vv;
    auto start = chrono::system_clock::now();
    
    std::uniform_int_distribution<> elem_distr;
    for (int i = 0; i < kElemCount; ++i) {
        vv.push_back(elem_distr(rng));
    }
    
    
    for (int i = 0; i < kModifCount; ++i) {
        std::uniform_int_distribution<> distr(0, vv.size()-1);
        auto ind = distr(rng);
        auto v = vv[ind];
        remove(vv.begin(), vv.end(), v);
        vv.pop_back();
        
        for (auto v : vv) {
            // do something here
        }
        
        auto elem = elem_distr(rng);
        auto it = std::upper_bound(vv.begin(), vv.end(), elem);
        vv.insert(it, elem);
        
        for (auto v : vv) {
            // do something again
        }
    }
    
    //std::set<uint64_t> vv
    
    
    
    
    
    
}


int main(int argc, char **argv) {
    cout << "working directory: " << argv[0] << endl; 
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
