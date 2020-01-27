#pragma once

namespace ant::graph {


template<class Func, bool directed>
void TraverseEdges(Count nodeCount, Func&& func);

template<bool directed, class Func, typename std::enable_if<directed==true>::type* = nullptr>
void TraverseEdges(Count nodeCount, Func&& func) {
    for (auto i = 0; i < nodeCount; ++i) {
        for (auto j = i+1; j < nodeCount; ++j) {
            func(i, j);
            func(j, i);
        }
    }
}

template<bool directed, class Func, typename std::enable_if<directed==false>::type* = nullptr>
void TraverseEdges(Count nodeCount, Func&& func) {
    for (auto i = 0; i < nodeCount; ++i) {
        for (auto j = i+1; j < nodeCount; ++j) {
            func(i, j);
        }
    }
}


template<class GraphBuilder>
typename GraphBuilder::GraphType BuildRandom(typename GraphBuilder::NodeType nodeCount, double completeness) {
    using NodeType = typename GraphBuilder::NodeType;

    std::vector<std::pair<NodeType, NodeType>> edges;
    TraverseEdges<GraphBuilder::GraphType::directed>(nodeCount, [&](NodeType from, NodeType to) {
        edges.emplace_back(from, to);
    });

    std::default_random_engine rng(time(0));
    std::shuffle(edges.begin(), edges.end(), rng);
    Count edgeCountNeeded = completeness*edges.size();
    edges.erase(edges.begin() + edgeCountNeeded, edges.end());

    GraphBuilder builder(nodeCount);
    for (auto [i, j] : edges) {
        builder.Add(i, j);
    }
    return builder.Build();
};


}