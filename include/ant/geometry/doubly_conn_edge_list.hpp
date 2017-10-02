#pragma once





template<typename VertexId, typename EdgeId, typename FaceId>
class DoublyConnEdgeList {

    struct HalfEdge;

    struct Vertex {
        VertexId id;
        list<HalfEdge>::iterator;
    };

    struct HalfEdge {
        EdgeId id;
        list<Vertex>::iterator origin;
        list<HalfEdge>::iterator twin, next, prev;
        list<Face>::iterator face;

    };

    struct Face {
        FaceId id;
        list<HalfEdge>::iterator outerComponent;
        vector<list<HalfEdge>::iterator> innerComponents;
    };

    list<Vertex> vertices;
    list<HalfEdge> halfEdges;
    list<Face> faces;

public:

};