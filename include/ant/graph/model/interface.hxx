


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
    <Range Advance Iterator> advance();         // required
};

class Edge {
    <EdgeDescriptor> descriptor();           // optional
    <EdgeValue> value();                     // optional

    // directed
    <Vertex> source();             // required
    <Vertex> target();             // required

    // undirected
    std::array<Vertex, 2> vertices(); // required
};

// for undirected graphs source and target don't make sense
// as much as out_edges and in_edges

// for undirected graphs mostly
class Advance {
    Edge edge()

    Vertex from();
    Vertex to();
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


// while iterator values providing descriptors is an interesting idea it forces
// two things:
// - edges and vertices have to have descriptors that I can search by
// - search by those descriptors has to be fast
// that's why we use lightweight objects supported by concepts instead
// providing flexibility for implementor

}