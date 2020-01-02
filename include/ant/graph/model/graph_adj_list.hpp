#pragma once

namespace ant::graph {


//Edge {
//NodeIndex nodeIndex;
//EdgeIndex edgeIndex;
//[[no_unique_address]] EdgeValue value;
//}
//
//Node {
//std::vector<Edge> edges;
//[[no_unique_address]] VertixValue value;
//}
//
//EdgeInfo {
//VertixIndex source;
//Index sourceEdgeIndex;
//
//VertixIndex target;
//}
//
//Graph {
//vector<Node> nodes;
//vector<EdgeInfo> edgeInfos;
//}

struct Graph_AdjList {

    struct Node {
        std::vector<Edge> edges;
        [[no_unique_address]] VertexValue value;
    };

    struct Edge {
        NodeIndex nodeIndex;
        EdgeIndex edgeIndex;
        [[no_unique_address]] EdgeValue value;
    };



};

}