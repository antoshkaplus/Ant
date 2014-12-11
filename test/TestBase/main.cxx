//
//  main.cpp
//  TestBase
//
//  Created by Anton Logunov on 7/10/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include <iostream>

#include "base.h"
#include "bigint.h"

using namespace ant::d2::grid;
using ant::bigint;

using namespace std;

void grid_iterator_test() {
    Grid<double> g(9,9);
    auto it = g.begin();
    while (it != g.end()) {
        it++;
        ++it;
        *it = 90;
    }
}

void bigint_test() {
    string str_0 = "   -39408568329562402658000000000000006395  ";
    string str_1 = " 043258239239642992456483924512523135534352342347529";
    string str_2 = "0";
    
    ant::bigint b_0(str_0);
    ant::bigint b_1(str_2);
    cout << b_0 << endl;
    cout << b_1 << endl;
    cout << b_0 * b_1;
}

void combinations() {
    ant::CombinationGenerator<unsigned char> c(5, 2);
    while (true) {
        
        cout << 
        if (!c.hasNext()) break;
        unsigned char t = c.next();
    }
}


int main(int argc, const char * argv[]) {
    bigint_test();
    return 0;
}
