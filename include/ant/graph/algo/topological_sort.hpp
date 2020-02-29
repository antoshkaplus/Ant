#pragma once

// TODO


template<class AdjacencyListPtr>
std::pair<std::vector<Index>, bool> TopologicalSort(const Graph<AdjacencyListPtr>& gr) {
    std::vector<Index> L;
    std::vector<Index> S;
    std::vector<Count> incoming_count(gr.node_count(), 0);
    for (Index n = 0; n < gr.node_count(); ++n) {
        for (auto m : gr.adjacent(n)) {
            ++incoming_count[m];
        }
    }
    for (Index n = 0; n < gr.node_count(); ++n) {
        if (incoming_count[n] == 0) S.push_back(n);
    }
    while (!S.empty()) {
        Index n = S.back();
        S.pop_back();
        L.push_back(n);
        for (auto m : gr.adjacent(n)) {
            if (--incoming_count[m] == 0) {
                S.push_back(m);
            }
        }
    }
    if (std::accumulate(incoming_count.begin(), incoming_count.end(), 0) > 0) {
        return {L, false};
    }
    return {L, true};
}
