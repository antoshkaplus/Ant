#pragma once

#pragma once

#include <variant>

#include "ant/core/core.hpp"

namespace ant::graph {

class Component {
    std::unordered_set<Index> nodes;
    std::vector<Index> inner_components;
    // nodes that are on the main borber
    std::vector<Index> border_nodes;

    // mapping from Node => Index in distance matrix
    std::unordered_map<Index, Index> pp;

    // I need distances from border node -> any other border node
    //                       border node -> border node of each component

    // between component border nodes????

    template <class Func>
    void ForEachBorderEdge(Func&& func)
    {
        func(from, to, to_component, edge_distance);
    }


    template <class Func>
    void ForEachBorderEdge(Index from_border_node, Func&& func)
    {

    }

    bool Contains(Index node) const {
        return nodes.count(node) != 0;
    }
};



template <typename T_Dist>
class MultiLevel_Builder {

    using Node = Index;
    using Component = Index;

    std::vector<Component> components;
    // we need to find leaf components for nodes


    // node index => leaf component index
    std::vector<Index> nodes_leaf_components;

    T_Dist no_value_reach = std::numeric_limits<T_Dist>::max();

    struct Item {
        Node node;
        Index component;
        T_Dist reach;
    };

    // probably should be inside some kind of method
    std::priority_queue<Item> items;

public:
    MultiLevel_Builder() {





    }


    void Build(const EdgedGraph<Index, Index>& graph, const std::vector<T_Dist>& edge_dist) {

        // it has to be recursive process
        // you bring in a component

        // in that component you pick one vertex and
        // shuffle all edges of it
        // pick an edge with the end inside this component
        // the ends become sources of two different components
        // use BFS to extend the sets based on edge length in both directions
        // after that put sets as inner components

        // check that no more nodes left, otherwise start a separate component from each point.

        // will also need to run BFS from each interior point with external edge to each other point with external edge
        // actually it probably can be done on the way back...

        // based on each inner component has to have distances set

        // so we just kind of use a subgraph, with all the distances into matrix


    }

};



// bidirectional search
template <typename T_Dist>
class MultiLevel_SP {

    using Node = Index;
    using Component = Index;

    std::vector<Component> components;
    // we need to find leaf components for nodes


    // node index => leaf component index
    std::vector<Index> nodes_leaf_components;

    std::vector<bool> source_reach;
    std::vector<T_Dist> reach;

    T_Dist no_value_reach = std::numeric_limits<T_Dist>::max();

    struct Item {
        Node node;
        Index component;
        T_Dist reach;
    };

    std::priority_queue<Item> items;

    bool target_found;

public:
    std::optional<T_Dist> Query(Node source, Node target) {
        // first go by source and then by target to connect with
        // the search by source



        items.clear();

        if (!target_found) return {};

        if (reach[target] != no_value_reach) return reach[target];

        return {};
    }

private:
    void ReachBorderNodes(Node node, Component component) {

    }

    bool SearchTargetFromSource() {

        auto leaf = nodes_leaf_components[source];

        reach[source] = 0;
        components.ForEachBorderEdge([&](Node from, Node to, Index to_component, T_Dist edge_distance) {
            if (from != source) throw std::runtime_error("can't be");

            items.emplace(to, to_component, edge_distance);
        });

        bool target_found = false;
        while (!items.empty()) {
            auto item = items.top();
            items.pop();

            if (reach[item.node] != no_value_reach) continue;
            reach[item.node] = item.reach;
            source_reach[item.node] = true;

            auto& c = components[item.component];
            if (c.Contains(target))
            {
                target_found = true;
                break;
            }

            ReachBorderNodes(item.node);
            components.ForEachBorderEdge([&](Node from, Node to, Index to_component, T_Dist edge_distance) {
                items.emplace(to, to_component, reach[from]+edge_distance);
            });
        }

    }

    void SearchFromTarget() {

        reach[target] = 0;
        components.ForEachBorderEdge([&](Node from, Node to, Index to_component, T_Dist edge_distance) {
            if (from != target) throw std::runtime_error("can't be");

            items.emplace(to, to_component, edge_distance);
        });

        // probably hide behind some function
        while (!items.empty()) {
            auto item = items.top();
            items.pop();

            if (reach[item.node] != no_value_reach) {
                if (source_reach[item.node]) return reach[item.node] + item.reach;
                continue;
            }
            reach[item.node] = item.reach;

            auto& c = components[item.component];

            ReachBorderNodes(item.node, item.component);
            c.ForEachBorderEdge([&](Node from, Node to, Index to_component, T_Dist edge_distance) {
                items.emplace(to, to_component, reach[from]+edge_distance);
            });
        }

    }

};



}