#pragma once

namespace ant::graph::model::adj_list {

template <typename Model>
class Edge {

    Model& model;
    VertexDescriptor source;
    VertexDescriptor target;

    // very similar to advance but for undirected graphs doesn't
    // provide direction

public:
    using VertexDescriptor = typename Model::VertexDescriptor;
    using EdgeDescriptor = typename Model::EdgeDescriptor;
    using VertexType = typename Model::VertexType;

    Edge(Model& model, VertexDescriptor source, VertexDescriptor target) {

    }

    // based on model
//    auto value();                     // optional


     source();             // required
    Model::VertexType target();             // required

    std::array<VertexTYpe, 2> vertices(); // required

};

}