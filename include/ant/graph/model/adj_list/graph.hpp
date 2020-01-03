#pragma once

#include "ant/core/core.hpp"
#include "ant/graph/graph_traits.hpp"

namespace ant::graph::model::adj_list {

// based on user config preferences we select the right implementation

// interesting how gonna pick mutator, probably also by config
// maybe kind of keep mutator through composition

template <typename Config,
    std::enable_if_t<is_directed_v<Config>, int> = 0> // and whole bunch more
class Graph : public Model_1<> { // bla bla
public:

    // will need to repeat all 'using declarations'
};

};
