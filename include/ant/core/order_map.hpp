#pragma once

#include "ant/core/core.hpp"


namespace ant {

// let's say we have an array of items ARR_1. somewhere there is another array E of indexes pointing to elements in ARR_1.
// at some point we've decided to reorder the array ARR_1 => ARR_2. we keep new order of indexes in the index array I_2.
// our goal now is to make a mapping out of this new array I_2, so we could get to the original items using element index:
// if i <- E, ARR_2[ I_2[i] ] = ARR_1[ i ]

// possible to use SSE to check that all sign bits are clear
template<class T, typename std::enable_if<std::is_signed<T>::value>::type* = nullptr>
void MakeOrderMap(std::vector<T>& order) {

    for (auto i = 0; i < order.size(); ++i) {
        if (order[i] < 0) continue;

        T newIndex = i;
        T oldIndex = order[i];
        for (;;) {
            auto index = oldIndex;
            auto& item = order[index];
            // save for the next iteration
            oldIndex = item;
            item = newIndex ^ -1;
            newIndex = index;
            if (i == newIndex) break;
        }
    }
    for (auto& i : order) {
        i ^= -1;
    }
}

template<class T>
class OrderMap {

    struct Item {
        T index, newIndex;

        Item() = default;
        Item(T index, T newIndex) : index(index), newIndex(newIndex) {}
    };

    std::vector<Item> map;

public:
    void Make(std::vector<T>& order) {
        map.clear();
        map.reserve(order.size());

        for (auto i = 0; i < order.size(); ++i) {
            map.emplace_back(order[i], i);
        }

        for (auto& p : map) {
            order[p.index] = p.newIndex;
        }
    }
};

}