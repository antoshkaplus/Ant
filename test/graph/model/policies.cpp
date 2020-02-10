
#include <fstream>
#include <array>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"
#include "ant/graph/policies.hpp"

namespace {

using namespace std;
using namespace ant;
using namespace ant::graph::policy;

TEST(Policy, Base) {
    static_assert(ant::graph::is_vertex_descriptor_index_v<BasePolicy>);
}

TEST(Policy, EdgeIndexDescriptor) {
    using Policy = Policy<EdgeIndexDescriptor<>>;
    static_assert(ant::graph::has_edge_descriptor_index<Policy>::value);
    static_assert(ant::graph::is_edge_descriptor_index_v<Policy>);
    static_assert(ant::graph::is_edge_descriptor_v<Policy>);
}



}

