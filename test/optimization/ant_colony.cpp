//
//  ant_colony.cpp
//  ANT
//
//  Created by Anton Logunov on 4/25/15.
//
//

#include <stdio.h>

#include "gtest/gtest.h"

#include "ant/optimization/tsp/tsp_ant_colony.hpp"
#include "ant/optimization/tsp/tsp_random_insertion.hpp"
#include "ant/optimization/tsp/tsp_farthest_insertion.hpp"
#include "ant/optimization/tsp/tsp_simplex_insertion.hpp"
#include "ant/core.h"

namespace {

using namespace ant::opt::tsp;
using namespace std;

TEST(TSP_AntColony, allin) {
    TSP_SimplexInsertion rand;
    TSP_AntColony ant;
    vector<Point> ps;
    ant.solve(ps);
    rand.solve(ps);
    
}

}