


namespace ant::graph::model {

class Vertex {
    <VertexDescriptor> descriptor();         // required
    <VertexValue> value();                   // optional

    // directed
    <Range of OutEdge Iterator> out_edges(); // optional
    <Range of InEdge Iterator> in_edges();   // optional

    // undirected
    <Range of Edge Iterator> edges();        // optional

    // most important one
    <Range Advance Iterator> next();         // required
};

class Edge {
    <EdgeDescriptor> descriptor();           // optional
    <EdgeValue> value();                     // optional

    // directed
    <VertexDescriptor> source();             // required
    <VertexDescriptor> target();             // required

    // undirected
    std::array<VertexDescriptor, 2> vertices(); // required
};

// for undirected graphs source and target don't make sense
// as much as out_edges and in_edges

// for undirected graphs mostly
class Advance {
    EdgeDescriptor edge_descriptor();           // optional
    VertexDescriptor from();                    // required
    VertexDescriptor to();                      // required
};

// those classes will probably host Graph object in
// them, gonna be inlined though.
// user better not keep those objects as is.


class Graph {
    <Range of Vertex Iterator> vertices();          // optional
    <Range of Edge Iterator> edges();               // required

    Vertex& vertex(VertexDescriptor descriptor);    // required
    Edge& edge(EdgeDescriptor descriptor);          // optional
};

// Vertex Iterator evaluates to Vertex
// Edge Iterator evaluates to Edge

// delegate knows how to name the vertex and what default value to use
<VertexDescriptor> AddVertex();
<VertexDescriptor> AddVertex(VertexValue);

// not for all
void RemoveVertex(VertexDescriptor);

<EdgeDescriptor> AddEdge(VertexDescriptor from, VertexDecriptor to);
<EdgeDescriptor> AddEdge(VertexDescriptor from, VertexDecriptor to, EdgeValue);

void RemoveEdge(EdgeDescriptor);

// SubgraphBuilder - supports only addition
// DerivativeBuilder

// SubgraphMutatorGuard
// DerivativeMutatorGuard

// SubgraphMutator
// DerivativeMutator

}