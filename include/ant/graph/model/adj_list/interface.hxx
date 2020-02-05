// private interface

class Model {

    std::vector<VertexInfo> vertices_info;   // required
    std::vector<EdgeInfo> edges_info;        // optional

    using Vertex;
};

class VertexInfo {

    std::vector<Adjacent> adjacent;  // required
    <VertexValue> value;             // optional

};

class Adjacent {

    <VertexDescriptor> vertex_descriptor;

    <EdgeDescriptor> edge_descriptor;
    <EdgeValue> edge_value;
};