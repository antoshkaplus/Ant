
#include "gtest/gtest.h"
#include "ant/graph/graph.hpp"
#include "ant/graph/component.hpp"


namespace {

using namespace std;
using namespace ant;
using namespace ant::graph;


TEST(Graph, Constructor) {
    {
        GraphBuilder<true, Index> builder(10);
        builder.Add(0, 1);
        auto g = builder.Build();
        g = Reverse(g);
    }
}

// TODO FIX

//TEST(Graph, Component) {
//    GraphBuilder<true, Index> builder(10);
//    builder.Add(0, 1);
//    auto g = builder.Build();
//
//    auto components = Kosaraju(g);
//}

}