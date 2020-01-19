
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/model/adj_list/model_1.hpp"
#include "ant/graph/model/adj_list/vertex_info_1.hpp"
#include "ant/graph/model/adj_list/vertex_1.hpp"
#include "ant/graph/model/adj_list/mutator_1.hpp"
#include "ant/graph/policies.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::model::adj_list;


TEST(Model_1, allin) {
    using namespace ant::graph::policy;

    using Model = Model_1<BasePolicy, VertexInfo_1<ant::Index>, Vertex_1, Mutator_1>;
    Model model;

    Model::Mutator mutator(model);
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