

#include <fstream>
#include <array>
#include <functional>
#include <set>

#include "gtest/gtest.h"

#include "ant/core.h"

namespace {
    
    using namespace ant;
    using namespace std;
    
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
    
}



