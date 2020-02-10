
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


TEST(Graph_Base, InterfaceCompiles) {
    Graph<ant::graph::policy::BasePolicy> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(10);
    mutator.AddEdge(0, 1);

    for (auto v : g.vertices()) {
        (void)v.advance();
        (void)v.descriptor();
    }
    auto v = g.vertex(0);
    for (auto a : v.advance()) {
        (void)a.to();
        (void)a.from();
        (void)a.edge();
    }
    for (auto e : g.edges()) {
        (void)e.vertices();
    }
}


TEST(Graph_EdgeIndex, InterfaceCompiles) {
    using namespace ant::graph::policy;
    Graph<Policy<EdgeIndexDescriptor<>>> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(10);
    mutator.AddEdge(0, 1);

    for (auto v : g.vertices()) {
        (void)v.advance();
        (void)v.descriptor();
    }
    auto v = g.vertex(0);
    for (auto a : v.advance()) {
        (void)a.to();
        (void)a.from();
        (void)a.edge();
    }
    for (auto e : g.edges()) {
        (void)e.vertices();
    }
    auto e = g.edge(0);
    e.descriptor();
}

TEST(Graph_Directed, InterfaceCompiles) {
    using namespace ant::graph::policy;

    Graph<Policy<Directed>> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(10);
    for (auto i = 1; i < 10; ++i) {
        mutator.AddEdge(i-1, i);
    }

    for (auto v : g.vertices()) {
        (void)v.advance();
        (void)v.descriptor();
    }
    auto v = g.vertex(0);
    for (auto a : v.advance()) {
        (void)a.to();
        (void)a.from();
        (void)a.edge();
    }
    for (auto e : g.edges()) {
        (void)e.from();
        (void)e.to();
    }
}


TEST(Graph_VertexValue, InterfaceCompiles) {
    using namespace ant::graph::policy;

    Graph<Policy<ant::graph::policy::VertexValue<int64_t>>> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(10);
    for (auto i = 1; i < 10; ++i) {
        mutator.AddEdge(i-1, i);
    }

    for (auto v : g.vertices()) {
        (void)v.advance();
        (void)v.descriptor();
        (void)v.value();
    }
    auto v = g.vertex(0);
    for (auto a : v.advance()) {
        (void)a.to();
        (void)a.from();
        (void)a.edge();
    }
    for (auto e : g.edges()) {
        (void)e.vertices();
    }
}


TEST(Graph_Directed_VertexValue, InterfaceCompiles) {
    using namespace ant::graph::policy;

    Graph<Policy<ant::graph::policy::Directed, ant::graph::policy::VertexValue<int64_t>>> g;

    decltype(g)::Mutator mutator(g);
    mutator.SetVertexCount(10);
    for (auto i = 1; i < 10; ++i) {
        mutator.AddEdge(i-1, i);
    }

    for (auto v : g.vertices()) {
        (void)v.advance();
        (void)v.descriptor();
        (void)v.value();
    }
    auto v = g.vertex(0);
    for (auto a : v.advance()) {
        (void)a.to();
        (void)a.from();
        (void)a.edge();
    }
    for (auto e : g.edges()) {
        (void)e.from();
        (void)e.to();
    }
}


}