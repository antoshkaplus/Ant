#pragma once

#include "ant/core/core.hpp"

namespace ant::graph {

//for each vertex v in vertices:
//  distance[v] := inf             // Initialize the distance to all vertices to infinity
//  predecessor[v] := null         // And having a null predecessor
//
//        distance[source] := 0              // The distance from the source to itself is, of course, zero
//
//// Step 2: relax edges repeatedly
//for i from 1 to size(vertices)-1:
//for each edge (u, v) with weight w in edges:
//if distance[u] + w < distance[v]:
//distance[v] := distance[u] + w
//        predecessor[v] := u
//
//// Step 3: check for negative-weight cycles
//for each edge (u, v) with weight w in edges:
//if distance[u] + w < distance[v]:
//error "Graph contains a negative-weight cycle"
//
//return distance[], predecessor[]


// WeightFunc extracts weight from the graph edge value
// now also need to get weight type itself

// or maybe just restrict for now, make certain requirements for the graph

// can we run adapter that will trasnform current edge value into another one ???

// with adapters I can't do many changes

template <typename Graph, typename WeightFunc>
void BellmanFord(const Graph& G, WeightFunc&& weight, typename Graph::VertixId source) {
    using WeightType = decltype(weight(G.adjacent_vertices(source).begin()->value));

    std::vector<WeightType> distances (G.vertix_count(), std::numeric_limits<WeightType>::min());



    // somehow I need to get
    // new vector for distances and predesessor

    // Graph::VertixMap<>

    for (auto v : G.vertices()) {

    }

}


}