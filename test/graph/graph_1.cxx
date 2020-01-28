
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/bfs.hpp"


namespace {
    
    using namespace std;
    using namespace ant;
    using namespace ant::graph;
    
    
    TEST(BFS, allin) {
        std::ifstream undir("./../data/graph/undir_0.txt");
        std::ifstream bfs("./../data/graph/test_bfs_0.txt");
        Count N;
        undir >> N;
        AdjacencyList<Index> adj(N);
        Index K;
        for (int i = 0; i < N; ++i) {
            undir >> K;
            for (int j = 0; j < K; ++j) {
                Index h;
                undir >> h;
                adj[i].push_back(h);
            }
        }
        auto g = CreateGraph(adj);
        Count T;
        bfs >> T;
        vector<int> time(N);
        vector<int> time_2(N);
        Count last_t = -1;
        for (int i = 0; i < T; ++i) {
            Index st;
            bfs >> st;
            fill(time.begin(), time.end(), -1);
            fill(time_2.begin(), time_2.end(), -1);
            time[st] = 0;
            for (int j = 0; j < N-1; ++j) {
                Index v, t;
                bfs >> v >> t;
                time[v] = t;
            }
            auto proc = [&] (Index i) {
                time_2[i] = ++last_t;
                return false;
            };
            BFS(g, st, proc);
            for (int i = 0; i < N; ++i) {
                for (int j = i+1; j < N; ++j) {
                    if (time[i] < time[j] && time_2[i] > time_2[j]) {
                        FAIL();
                    }
                }
            }
        }
    }
    
    
    TEST(TopologicalSort, allin) {
        AdjacencyList<Index> adj(6);
        adj[2].push_back(3);
        adj[3].push_back(1);
        adj[4].push_back(0);
        adj[4].push_back(1);
        adj[5].push_back(0);
        adj[5].push_back(2);
        auto g = CreateGraph(adj);
        auto order = TopologicalSort(g);
        if (!order.second) {
            cout << "TopologicalSort graph has cycles" << endl;
        }
        for (auto o : order.first) {
            cout << o << " ";
        }
    }
}