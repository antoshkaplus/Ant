
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/core/range/range.hpp"
#include "ant/graph/model/adj_vec/graph.hpp"
#include "ant/graph/policies.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::model::adj_vec;


TEST(Graph_EdgeIndex, Edges) {
    using namespace ant::graph::policy;
    Graph<Policy<EdgeIndexDescriptor<>>> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(3);

    mutator.AddEdge(0, 1);
    mutator.AddEdge(1, 2);
    mutator.AddEdge(2, 0);

    ASSERT_EQ(ant::core::range::Count(g.vertices()), 3);
    ASSERT_EQ(ant::core::range::Count(g.edges()), 3);
}

}