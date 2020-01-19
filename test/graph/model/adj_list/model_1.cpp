
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/model/adj_list/graph.hpp"
#include "ant/graph/policies.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::model::adj_list;


TEST(Model_1, allin) {
    using namespace ant::graph::policy;

    Graph<ant::graph::policy::BasePolicy> model;

    decltype(model)::Mutator mutator(model);
    mutator.SetVertexCount(10);
    for (auto i = 1; i < 10; ++i) {
        mutator.AddEdge(i-1, i);
    }

//    for (auto v : model.vertices()) {
//        Println(cout, v.descriptor());
//        for (auto a : v.advance()) {
//            Println(cout, a.from(), a.to());
//        }
//    }
}

TEST(Model_1, allin_directed) {
    using namespace ant::graph::policy;

    Graph<Policy<ant::graph::policy::Directed>> model;

    decltype(model)::Mutator mutator(model);
    mutator.SetVertexCount(10);
    for (auto i = 1; i < 10; ++i) {
        mutator.AddEdge(i-1, i);
    }

//    for (auto v : model.vertices()) {
//        Println(cout, v.descriptor());
//        for (auto a : v.advance()) {
//            Println(cout, a.from(), a.to());
//        }
//    }
}

}