#pragma once




// maybe better use vector.
template<typename VertexId, typename EdgeId, typename FaceId>
class DoublyConnEdgeList {

    struct HalfEdge;

    struct Vertex {
        VertexId id;
        std::list<HalfEdge>::iterator randomEdge;
    };

    struct HalfEdge {
        EdgeId id;
        std::list<Vertex>::iterator origin;
        std::list<HalfEdge>::iterator twin, next, prev;

        // face is probably not needed at all
        std::list<Face>::iterator face;

    };

    struct Face {
        FaceId id;
        std::list<HalfEdge>::iterator outerComponent;
        std::vector<list<HalfEdge>::iterator> innerComponents;
    };

    std::list<Vertex> vertices;
    std::list<HalfEdge> halfEdges;
    std::list<Face> faces;

public:

    std::list<Vertex>::const_iterator AddVertex(VertexId id) {
        return vertices.emplace(vertices.end(), id, halfEdges.end());
    }

    void AddEdge(EdgeId id, std::list<Vertex>::const_iterator v_1, std::list<Vertex>::const_iterator v_2) {

        auto e_1 = halfEdges.emplace(id, v_1);
        auto e_2 = halfEdges.emplace(id, v_2);



    }

};