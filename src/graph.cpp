


#include "ant/graph/graph.hpp"


namespace ant {
    
namespace graph {
    

NodeAdjacencyList EdgesToAdjacencyList(const std::vector<Edge>& edges, size_t node_count) {
    NodeAdjacencyList adj_list(node_count);
    for (const Edge& p : edges) {
        adj_list[p[0]].push_back(p[1]);
        adj_list[p[1]].push_back(p[0]);
    }
    return adj_list;
}    
    
    
Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<const NodeAdjacencyList>& ptr) {
    return Graph<std::shared_ptr<const NodeAdjacencyList>>(ptr);
}

Graph<std::shared_ptr<const NodeAdjacencyList>> CreateGraph(const std::shared_ptr<NodeAdjacencyList>& ptr) {
    return Graph<std::shared_ptr<const NodeAdjacencyList>>(std::const_pointer_cast<const NodeAdjacencyList>(ptr)); 
}

Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList* ptr) {
    return Graph<const NodeAdjacencyList*>(ptr);
}

Graph<const NodeAdjacencyList*> CreateGraph(const NodeAdjacencyList& ptr) {
    return CreateGraph(&ptr);
}


} // end namespace graph

} // end namespace ant