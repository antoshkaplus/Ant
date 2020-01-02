#pragma once

#include "ant/core/core.hpp"

namespace ant::graph::model::adj_list {

// if "Directed" can store EdgeValue with Next,
// in case of undirected edge values have to be stored outside
// inside vector. and you get values by edge_descriptor

class Next {
    VertexDescriptor vertex_descriptor;
    EdgeDescriptor edge_descriptor;
};

class NextVector {

    VertexValueType value;
    std::vector<NextType>

};

class VerticesEdges {

    std::vector<NextVectorType> next_;

public:



};


}